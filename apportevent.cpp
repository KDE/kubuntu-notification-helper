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

#include "apportevent.h"

#include <KProcess>
#include <KToolInvocation>

ApportEvent::ApportEvent( QObject* parent, QString name)
    : Event(parent, name)
{}

ApportEvent::~ApportEvent()
{}

void ApportEvent::show()
{
    QPixmap icon = KIcon( "apport" ).pixmap( 48, 48 );
    QString text( i18nc( "Notification when apport detects a crash", "An application has crashed on your system (now or in the past)" ) );
    QStringList actions;
    actions << i18nc( "Opens a dialog with more details", "Details" );
    actions << i18nc( "User declines an action", "Ignore" );
    actions << i18nc( "User indicates he never wants to see this notification again", "Never show again" );

    Event::show(icon,text,actions);
}

void ApportEvent::run()
{
   KToolInvocation::kdeinitExec("/usr/share/apport/apport-kde");
}

// TODO: what to do with this? doesnt have any use at this point
// int ApportEvent::checkApport()
// {
//     KProcess *apportProcess = new KProcess();
//     apportProcess->setProgram( QStringList() << "/usr/share/apport/apport-checkreports" );
// 
//     return apportProcess->execute();
// }

#include "apportevent.moc"
