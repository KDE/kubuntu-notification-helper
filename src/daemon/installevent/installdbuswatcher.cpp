/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "installdbuswatcher.h"
#include "restrictedinstalladaptor.h"

// Qt includes
#include <QtDBus/QDBusConnection>
#include <QtCore/QString>

InstallDBusWatcher::InstallDBusWatcher(QObject* parent)
        : QObject(parent)
{
    new RestrictedInstallAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/org/kubuntu/restrictedInstall", this);
    dbus.registerService("org.kubuntu.restrictedInstall");
}

InstallDBusWatcher::~InstallDBusWatcher()
{}

bool InstallDBusWatcher::installRestricted(const QString &application, const QString &package)
{
    emit installRestrictedCalled(application, package);
    return true;
}
