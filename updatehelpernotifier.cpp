/***************************************************************************
 *   Copyright (C) 2009 by Jonathan Thomas <echidnaman@kubuntu.org>        *
 *                                                                         *
 * This is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License version 3   *
 * only, as published by the Free Software Foundation.                     *
 *                                                                         *
 * This is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Lesser General Public License version 3 for more details            *
 * (a copy is included in the LICENSE file that accompanied this code).    *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public License*
 * version 3 along with Update Helper Notifier.  If not, see               *
 * <http://www.gnu.org/licenses/> for a copy of the LGPLv3 License.        *
 ***************************************************************************/

#include "updatehelpernotifier.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

// KDE
#include <KConfig>
#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KNotification>
#include <KProcess>

// Include for the sleep() function
#include <unistd.h>


UpdateHelperNotifier::UpdateHelperNotifier( QObject* parent )
    : QObject( parent )
    , dirWatch( 0 )
    , apportDirWatch( 0 )
    , apportNotifyShowing( false )
{
// FIXME Crashes and I don't know why
//     KConfig config("updatehelpernotifier");
//     KConfigGroup notifyGroup( &config, "Notify" );
//     showRestartNotification = notifyGroup.readEntry( "ShowRestartNotification", "True" );

    showRestartNotification = true;

    if ( showRestartNotification )
    {
        dirWatch = new KDirWatch( this );
        dirWatch->addFile( "/var/lib/update-notifier/dpkg-run-stamp" );
        connect( dirWatch, SIGNAL( created( const QString & ) ), this, SLOT( aptDirectoryChanged() ) );
    }

    if ( QFile::exists( "/usr/share/apport/apport-kde" ) || QFile::exists( "/usr/share/apport/apport-gtk" ) )
    {
        apportDirWatch =  new KDirWatch( this );
        apportDirWatch->addDir( "/var/crash/" );
        apportNotifyShowing = false;
        connect( apportDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( apportDirectoryChanged() ) );

        // Force check since we just started up and there might have been crashes on reboot
        bool systemCheck = false;
        int result = checkApport( systemCheck );
        if ( result == 0 )
            apportDirectoryChanged();
    }
}

UpdateHelperNotifier::~UpdateHelperNotifier()
{

}

void UpdateHelperNotifier::aptDirectoryChanged()
{
    showRestartNotification = true;

    if ( showRestartNotification && QFile::exists( "/var/lib/update-notifier/dpkg-run-stamp") && QFile::exists( "/var/run/reboot-required" ) )
    {
        QPixmap pix = KIcon( "system-reboot" ).pixmap( 48, 48 );

        KNotification *notify = new KNotification( "Restart", 0, KNotification::Persistent );
        notify->setText( i18nc( "Notification when the upgrade requires a restart", "A system restart is required" ) );
        notify->setPixmap( pix );

        QStringList actions;
        actions << i18nc( "Restart the computer", "Restart" );
        actions << i18nc( "User declines an action", "Not now" );

        notify->setActions( actions );
        connect( notify, SIGNAL( action1Activated() ), this, SLOT( restartActivated() ) );
        notify->sendEvent();
    }
}

void UpdateHelperNotifier::apportDirectoryChanged()
{
    if ( !apportNotifyShowing )
    {
        // We could be too fast for apport,  so wait a bit before showing the notification
        sleep ( 2 );
        QPixmap pix = KIcon( "apport" ).pixmap( 48, 48 );

        KNotification *apportNotify = new KNotification( "Restart", 0, KNotification::Persistent );
        apportNotify->setText( i18nc( "Notification when apport detects a crash", "An application has crashed on your system (now or in the past)" ) );
        apportNotify->setPixmap( pix );

        QStringList actions;
        actions << i18nc( "Opens a dialog with more details", "Details" );
        actions << i18nc( "User declines action, closing the notification", "Ignore" );

        apportNotify->setActions( actions );
        connect( apportNotify, SIGNAL( action1Activated() ), this, SLOT( runApport() ) );
        connect( apportNotify, SIGNAL( action2Activated() ), this, SLOT( apportNotifyClosed() ) );
        connect( apportNotify, SIGNAL( closed() ), this, SLOT( apportNotifyClosed() ) );
        apportNotify->sendEvent();

        apportNotifyShowing = true;
    }
}

void UpdateHelperNotifier::restartActivated()
{
    // 1,1,3 == ShutdownConfirmYes ShutdownTypeReboot ShutdownModeInteractive - see README.kworkspace for other possibilities
    KProcess::startDetached( QStringList() << "qdbus" << "org.kde.ksmserver" << "/KSMServer" << "org.kde.KSMServerInterface.logout" << "1" << "1" << "3" );
}

void UpdateHelperNotifier::runApport()
{
    bool systemCheck = true;
    int result = checkApport( systemCheck );
    if ( result == 0 )
        KProcess::startDetached( QStringList() << "kdesudo" << "/usr/share/apport/apport-kde" );
    else
        KProcess::startDetached("/usr/share/apport/apport-kde");
}

int UpdateHelperNotifier::checkApport( bool system )
{
    KProcess *apportProcess = new KProcess();

    if ( system )
        apportProcess->setProgram( QStringList() << "/usr/share/apport/apport-checkreports" << "--system" );
    else
        apportProcess->setProgram( QStringList() << "/usr/share/apport/apport-checkreports" );

    return apportProcess->execute();
}

void UpdateHelperNotifier::apportNotifyClosed()
{
    apportNotifyShowing = false;
}

#include "updatehelpernotifier.moc"
