/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

#include "rebootevent.h"

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
    actions << i18nc( "User declines an action", "Not now" );
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
