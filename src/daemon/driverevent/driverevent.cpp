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

#include <QTimer>
#include <QDBusConnection>

#include <KAboutData>
#include <KNotification>

#include <LibQApt/Backend>

DriverWatcher::DriverWatcher(QObject* parent, QString name)
: Event(parent, name)
{
    KAboutData aboutData("drivermanager-notifier", "drivermanager-notifier",
                         ki18n("Driver Manager Notification Daemon"),
                         global_s_versionStringFull, ki18n("A Notification Daemon for the Driver Manager KCM"),
                         KAboutData::License_GPL,
                         ki18n("(C) 2014 Rohan Garg"),
                         KLocalizedString(), "http://kubuntu.org");
    
    // Lazy init to not cause excessive load when starting kded.
    // Also wait 2 minutes before actually starting the init as update
    // notifications are not immediately relevant after login and there is already
    // enough stuff going on without us. 2 minutes seems like a fair time as
    // the system should have calmed down by then.
    QTimer::singleShot(1000*60*2 /* 2 minutes to start */, this, SLOT(init()));
    
}

void DriverWatcher::init()
{
    m_aptBackend = new QApt::Backend(this);
    if (m_aptBackend->init()) {
        m_manager = new OrgKubuntuDriverManagerInterface("org.kubuntu.DriverManager", "/DriverManager", QDBusConnection::sessionBus());
        if (m_manager->isValid()) {
            m_manager->getDriverDict(false);
            connect(m_manager, SIGNAL(dataReady(QVariantMapMap)), SLOT(driverDictFinished(QVariantMapMap)), Qt::UniqueConnection);
        }
    }
}

void DriverWatcher::driverDictFinished(QVariantMapMap data)
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

void DriverWatcher::driverMapFinished(QDBusPendingCallWatcher* data)
{
    if (!data->isError()) {
        QDBusPendingReply<QVariantMapMap> mapReply = *data;
        QVariantMapMap map = mapReply.value();
        Q_ASSERT(map);
        Q_FOREACH (const QString &key, map) {
            QApt::Package *pkg = m_aptBackend->package(key);
            if (pkg) {
                if (!pkg->isInstalled() && map[key]["recommended"].toBool()) {
                    m_showNotification = true;
                }
            }
        }
    }
    
    m_showNotification = true;
    if (m_showNotification) {
        KNotification notify;
        notify.setComponentData(KComponentData("drivermanager-notifier"));
        notify.sendEvent();
    }
}
