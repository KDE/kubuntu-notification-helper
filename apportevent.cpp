/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

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
    QString text( i18nc( "Notification when apport detects a crash", "An application has crashed on your /esystem (now or in the past)" ) );
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

int ApportEvent::checkApport()
{
    KProcess *apportProcess = new KProcess();
    apportProcess->setProgram( QStringList() << "/usr/share/apport/apport-checkreports" );

    return apportProcess->execute();
}

#include "apportevent.moc"
