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


DriverEvent::DriverEvent(QObject *parent, QString name)
    : Event(parent, name)
    , m_showNotification(false)
    , m_aptBackendInitialized(false)
{
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
        return;
    }
    m_manager = new OrgKubuntuDriverManagerInterface("org.kubuntu.DriverManager", "/DriverManager", QDBusConnection::sessionBus());

    if (m_manager->isValid()) {
        m_manager->getDriverDict(false);
        connect(m_manager, SIGNAL(dataReady(QVariantMapMap)), SLOT(driverDictFinished(QVariantMapMap)), Qt::UniqueConnection);
    }
}


void DriverEvent::driverDictFinished(QVariantMapMap data)
{
    if (data.isEmpty()) {
        return;
    }

    Q_FOREACH(const QString &key,data.keys()) {
        QDBusPendingReply<QVariantMapMap> driverForDeviceMap = m_manager->getDriverMapForDevice(key);
        QDBusPendingCallWatcher *async = new QDBusPendingCallWatcher(driverForDeviceMap, this);
        connect(async, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(driverMapFinished(QDBusPendingCallWatcher*)));
    }
}

void DriverEvent::driverMapFinished(QDBusPendingCallWatcher *data)
{
    if (!data->isError()) {
        QDBusPendingReply<QVariantMapMap> mapReply = *data;
        QVariantMapMap map = mapReply.value();

        Q_FOREACH (const QString &key, map.keys()) {
            if (map[key]["recommended"].toBool()) {
                QApt::Package *pkg = m_aptBackend->package(key);
                if (pkg) {
                    if (!pkg->isInstalled()) {
                        m_showNotification = true;
                        break;
                    }
                }
            }
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

