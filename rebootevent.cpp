/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
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

// KDE includes
#include <KProcess>

RebootEvent::RebootEvent( QObject* parent, QString name)
    : Event(parent, name)
{}

RebootEvent::~RebootEvent()
{}

void RebootEvent::show()
{

    QPixmap icon = KIcon( "system-reboot" ).pixmap( 48, 48 );
    QString text(i18nc( "Notification when the upgrade requires a restart", "A system restart is required" ) );
    QStringList actions;
    actions << i18nc( "Restart the computer", "Restart" );
    actions << i18nc( "User declines an action", "Ignore" );
    actions << i18nc( "User indicates he never wants to see this notification again", "Never show again" );
    kDebug()<<"rebootevent";
    Event::show(icon,text,actions);
}

void RebootEvent::run()
{
    // 1,1,3 == ShutdownConfirmYes ShutdownTypeReboot ShutdownModeInteractive - see README.kworkspace for other possibilities
    KProcess::startDetached( QStringList() << "qdbus" << "org.kde.ksmserver" << "/KSMServer" << "org.kde.KSMServerInterface.logout" << "1" << "1" << "3" );
    Event::run();
}

#include "rebootevent.moc"
