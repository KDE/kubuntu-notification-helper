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

#ifndef DRIVEREVENT_H
#define DRIVEREVENT_H

#include "../event.h"
#include "drivermanagerdbustypes.h"

namespace QApt {
    class Backend;
}

class OrgKubuntuDriverManagerInterface;
class QDBusPendingCallWatcher;

class DriverEvent : public Event
{
    Q_OBJECT
public:
    DriverEvent(QObject* parent);

public Q_SLOTS:
    void show();

private:
    QApt::Backend *m_aptBackend;
    OrgKubuntuDriverManagerInterface *m_manager;
    bool m_showNotification;
    QStringList m_missingPackages;
    bool m_aptBackendInitialized;

private Q_SLOTS:
    void onDevicesReady(QDBusPendingCallWatcher *call);
    void run();
    void updateFinished();

};

#endif // DRIVEREVENT_H
