/*
 * Copyright 2014  Rohan Garg <rohan@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "driverevent.h"
#include <drivermanager_interface.h>

#include <QDBusConnection>

#include <LibQApt/Backend>

#include <KToolInvocation>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>

DriverEvent::DriverEvent(QObject *parent, QString name)
    : Event(parent, name)
    , m_showNotification(false)
    , m_aptBackendInitialized(false)
{
    qDBusRegisterMetaType<DeviceList>();

    m_aptBackend = new QApt::Backend(this);
    if (!m_aptBackend->init()) {
        kWarning() << m_aptBackend->initErrorMessage();
        m_aptBackendInitialized = false;
    }
    m_aptBackendInitialized = true;
}


void DriverEvent::show()
{
    if (m_aptBackendInitialized) {
        if(m_aptBackend->xapianIndexNeedsUpdate()) {
            m_aptBackend->updateXapianIndex();
            connect(m_aptBackend, SIGNAL(xapianUpdateFinished()), SLOT(updateFinished()));
        } else {
            updateFinished();
        }
    }
}

void DriverEvent::updateFinished()
{
    if (!m_aptBackend->openXapianIndex()) {
        kDebug() << "Xapian update could not be opened, probably broken.";
        return;
    }

    m_manager = new OrgKubuntuDriverManagerInterface("org.kubuntu.DriverManager", "/DriverManager", QDBusConnection::sessionBus());

    QDBusPendingReply<DeviceList> reply = m_manager->devices();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(onDevicesReady(QDBusPendingCallWatcher*)));
}

void DriverEvent::onDevicesReady(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<DeviceList> reply = *call;

    if (reply.isError()) {
        kDebug() << "got dbus error; abort";
        return;
    }

    DeviceList devices = reply.value();
    call->deleteLater(); // deep copy, delete caller

    kDebug() << "data " << devices;

    KConfig driver_manager("kcmdrivermanagerrc");
    KConfigGroup pciGroup( &driver_manager, "PCI" );

    foreach (Device device, devices) {
        if (pciGroup.readEntry(device.id) != QLatin1String("true")) {
            // Not seen before, check whether we have recommended drivers.
            for (int i = 0; i < device.drivers.length(); ++i) {
                // Supposedly Driver is not a pod due to ctor, so it can't
                // be fully used by QList :'<
                // Manually iter instead.
                Driver driver = device.drivers.at(i);
                if (driver.recommended) {
                    QApt::Package *package = m_aptBackend->package(driver.packageName);
                    if (package) {
                        if (!package->isInstalled()) {
                            m_showNotification = true;
                            break;
                        }
                    }
                }
            }
        } else {
            kDebug() << device.id << "has already been processed by the KCM";
        }
    }

    if (m_showNotification) {
        QString icon = QString("hwinfo");
        QString text(i18nc("Notification when additional packages are required for activating proprietary hardware",
                           "Proprietary drivers might be required to enable additional features"));
        QStringList actions;
        actions << i18nc("Launches KDE Control Module to manage drivers", "Manage Drivers");
        actions << i18nc("Button to dismiss this notification once", "Ignore for now");
        actions << i18nc("Button to make this notification never show up again",
                         "Never show again");
        Event::show(icon, text, actions);
    }
}

void DriverEvent::run()
{
    KToolInvocation::kdeinitExec("kcmshell4", QStringList() << "kcm_driver_manager");
    Event::run();
}

