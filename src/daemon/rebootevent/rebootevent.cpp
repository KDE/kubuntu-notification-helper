/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
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

#include "rebootevent.h"

#include <KProcess>

RebootEvent::RebootEvent(QObject* parent, QString name)
        : Event(parent, name)
{}

RebootEvent::~RebootEvent()
{}

void RebootEvent::show()
{
    QPixmap icon = KIcon("system-reboot").pixmap(NOTIFICATION_ICON_SIZE);
    QString text(i18nc("Notification when the upgrade requires a restart", "A system restart is needed to complete the update process"));
    QStringList actions;
    actions << i18nc("Restart the computer", "Restart");
    actions << i18nc("User declines an action", "Ignore");
    actions << i18nc("User indicates he never wants to see this notification again", "Ignore forever");

    Event::show(icon, text, actions);
}

void RebootEvent::run()
{
    // 1,1,3 == ShutdownConfirmYes ShutdownTypeReboot ShutdownModeInteractive
    KProcess::startDetached(QStringList() << "qdbus" << "org.kde.ksmserver" << "/KSMServer" << "org.kde.KSMServerInterface.logout" << "1" << "1" << "3");
    Event::run();
}

#include "rebootevent.moc"
