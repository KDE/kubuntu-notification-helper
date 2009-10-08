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
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

// KDE
#include <KConfigGroup>
#include <KDirWatch>
#include <KDebug>
#include <KGlobal>
#include <KIcon>
#include <KLocale>
#include <KNotification>
#include <KProcess>
#include <KVBox>

// Lower level includes for sleep, dirent
#include <unistd.h>
#include <dirent.h>


UpdateHelperNotifier::UpdateHelperNotifier( QObject* parent )
    : QObject( parent )
    , apportNotifyShowing( false )
    , parsedHookMap()
    , dialog( 0 )
{
    KConfig cfg( "updatehelpernotifier" );
    KConfigGroup notifyGroup( &cfg, "Notify" );
    showRestartNotification = notifyGroup.readEntry( "ShowRestartNotification", true );

    if ( showRestartNotification )
    {
        KDirWatch *stampDirWatch = new KDirWatch( this );
        stampDirWatch->addFile( "/var/lib/update-notifier/dpkg-run-stamp" );
        connect( stampDirWatch, SIGNAL( created( const QString & ) ), this, SLOT( aptDirectoryChanged() ) );
    }

    if ( QFile::exists( "/usr/share/apport/apport-kde" ) || QFile::exists( "/usr/share/apport/apport-gtk" ) )
    {
        KDirWatch *apportDirWatch =  new KDirWatch( this );
        apportDirWatch->addDir( "/var/crash/" );
        apportNotifyShowing = false;
        connect( apportDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( apportDirectoryChanged() ) );

        // Force check since we just started up and there might have been crashes on reboot
        bool systemCheck = false;
        int result = checkApport( systemCheck );
        if ( result == 0 )
            apportDirectoryChanged();
    }

    KDirWatch *hooksDirWatch = new KDirWatch( this );
    hooksDirWatch->addDir( "/var/lib/update-notifier/user.d/" );
    connect( hooksDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( hooksDirectoryChanged() ) );
}

UpdateHelperNotifier::~UpdateHelperNotifier()
{
    delete dialog;
}

void UpdateHelperNotifier::aptDirectoryChanged()
{
    if ( QFile::exists( "/var/lib/update-notifier/dpkg-run-stamp") && QFile::exists( "/var/run/reboot-required" ) )
    {
        QPixmap pix = KIcon( "system-reboot" ).pixmap( 48, 48 );

        KNotification *notify = new KNotification( "Restart", 0, KNotification::Persistent );
        notify->setComponentData(KComponentData("updatehelpernotifier"));
        notify->setText( i18nc( "Notification when the upgrade requires a restart", "A system restart is required" ) );
        notify->setPixmap( pix );

        QStringList actions;
        actions << i18nc( "Restart the computer", "Restart" );
        actions << i18nc( "User declines an action", "Not now" );
        actions << i18nc( "User indicates he never wants to see this notification again", "Never show again" );

        notify->setActions( actions );
        connect( notify, SIGNAL( action1Activated() ), this, SLOT( restartActivated() ) );
        connect( notify, SIGNAL( action3Activated() ), this, SLOT( disableRestartNotification() ) );
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
        apportNotify->setComponentData(KComponentData("updatehelpernotifier"));
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

void UpdateHelperNotifier::hooksDirectoryChanged()
{
    QStringList fileList;

    struct dirent **namelist;
    int i,n;

    n = scandir( "/var/lib/update-notifier/user.d/", &namelist, 0, alphasort );
    if (n > 0)
    {
        for ( i = 0; i < n; i++ )
        {
            fileList << namelist[i]->d_name;
            free( namelist[i] );
        }
    }

    foreach ( QString fileName, fileList ) {
        QMap< QString, QString > fileResult = processUpgradeHook( fileName );
        // if not empty, add parsed hook to the list of parsed hooks
        if ( !fileResult.isEmpty() )
        {
            fileResult["fileName"] = fileName;
            parsedHookMap[fileName] = fileResult;
        }
    }

    if ( !parsedHookMap.isEmpty() )
    {
        QPixmap pix = KIcon( "help-hint" ).pixmap( 48, 48 );

        KNotification *notify = new KNotification( "Restart", 0, KNotification::Persistent );
        notify->setComponentData(KComponentData("updatehelpernotifier"));
        notify->setText( i18n( "Software upgrade notifications are available" ) );
        notify->setPixmap( pix );

        QStringList actions;
        actions << i18nc( "Opens a dialog with more details", "Details" );
        actions << i18nc( "User declines action, closing the notification", "Ignore" );

        notify->setActions( actions );
        connect( notify, SIGNAL( action1Activated() ), this, SLOT( hooksActivated() ) );
        notify->sendEvent();
    }
}

void UpdateHelperNotifier::restartActivated()
{
    // 1,1,3 == ShutdownConfirmYes ShutdownTypeReboot ShutdownModeInteractive - see README.kworkspace for other possibilities
    KProcess::startDetached( QStringList() << "qdbus" << "org.kde.ksmserver" << "/KSMServer" << "org.kde.KSMServerInterface.logout" << "1" << "1" << "3" );
}

void UpdateHelperNotifier::disableRestartNotification()
{
    KConfig cfg( "updatehelpernotifier" );
    KConfigGroup notifyGroup( &cfg, "Notify" );
    notifyGroup.writeEntry( "ShowRestartNotification", false );
    showRestartNotification = false;
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

QMap<QString, QString> UpdateHelperNotifier::processUpgradeHook( QString fileName )
{
    QMap< QString, QString > fileInfo;
    QMap< QString, QString > emptyMap;

    // Toss crap from the directory lister (current dir and one-level-up items)
    if ( fileName.startsWith( '.' ) )
        return emptyMap;

    // Open the upgrade hook file
    QFile hookFile("/var/lib/update-notifier/user.d/" + fileName );

    /* Parsing magic. (https://wiki.kubuntu.org/InteractiveUpgradeHooks)
    Read the hook to a QString, split it up at each line. Then if a colon
    is in the line, split it into two fields, they key and the value.
    the line. Make a key/value pair in our fileInfo map. If the line beings
    with a space, append it to the value of the most recently added key.
    (As seen in the case of the upgrade hook description field) */
    if ( hookFile.open( QFile::ReadOnly ) )
    {
        QTextStream stream( &hookFile );
        QString streamAllString = stream.readAll();
        QStringList streamList = streamAllString.split( '\n' );
        foreach ( QString streamLine, streamList )
        {
            bool containsColon = streamLine.contains( ':' );
            bool startsWithSpace = streamLine.startsWith( ' ' );
            if ( containsColon )
            {
                QStringList list = streamLine.split( ": " );
                QString key = list.first();
                fileInfo[key] = list.last();
            }
            else if ( startsWithSpace )
            {
                QString previousDescription = fileInfo[ "Description" ];
                kDebug() << "previousDescription == " << previousDescription;
                fileInfo[ "Description" ] = ( previousDescription + streamLine );
            }
            else if ( streamLine.isEmpty() )
            {
                // Handle empty newline(s) at the end of files
                continue;
            }
            else
            {
                // Not an upgrade hook or malformed
                return emptyMap;
            }
        }
    }

    // TODO: Check if already shown, keep track via KConfig
    if ( fileInfo.contains( "DontShowAfterReboot" ) )
    {
        if ( fileInfo.value("DontShowAfterReboot") == "True" )
        {
            QFile uptimeFile( "/proc/uptime" );
            if ( uptimeFile.open( QFile::ReadOnly ) )
            {
                QTextStream stream( &uptimeFile );
                QString uptimeLine = stream.readLine();
                QStringList uptimeStringList = uptimeLine.split(' ');
                // We don't need the last part of /proc/uptime
                uptimeStringList.removeLast();
                QString uptimeString = uptimeStringList.first();
                float uptime = uptimeString.toFloat();
                const QDateTime now = QDateTime::currentDateTime();

                QDateTime statTime = QFileInfo( "/var/lib/update-notifier/user.d/" + fileName ).lastModified();
                // kDebug() << "uptime == " << uptime << " now == " << now.toTime_t() << " statTime == " << statTime.toTime_t();

                if ( uptime > 0 && ( ( now.toTime_t() - statTime.toTime_t() ) > uptime ) )
                {
                    return emptyMap;
                }
            }
        }
    }

    if ( fileInfo.contains( "DisplayIf") )
    {
        KProcess *displayIfProcess = new KProcess();
        displayIfProcess->setProgram( fileInfo.value("DisplayIf" ) );

        int programResult = displayIfProcess->execute();
        if ( programResult != 0 )
            return emptyMap;
    }

    return fileInfo;
}

void UpdateHelperNotifier::hooksActivated()
{
    dialog = new KPageDialog;
    dialog->setCaption( "Update Information" );
    dialog->setWindowIcon( KIcon( "help-hint" ) );
    dialog->setButtons( KDialog::Ok );
    connect( dialog, SIGNAL( okClicked() ), SLOT( cleanUpDialog() ) );

    // Take the parsed upgrade hook(s) and put them in pages
    QMap< QString, QMap< QString, QString> >::iterator i;
    for (i = parsedHookMap.begin(); i !=  parsedHookMap.end(); ++i)
    {
        // Any way to do this without copying this to a new QMap?
        QMap< QString, QString > parsedHook = *i;

        KVBox *vbox = new KVBox();

        // FIXME: Figure out how to grab the user's language, don't hardcode
//         QString language = KGlobal::locale::language();
        QString language = "es";

        // Get a translated name if possible
        QString name;
        if ( parsedHook.contains( "Name-" + language ) )
            name = parsedHook.value( "Name-" + language );
        else
        {
            QMap<QString, QString>::const_iterator nameIter = parsedHook.constFind("Name");
            while (nameIter != parsedHook.end() && nameIter.key() == "Name")
            {
                name = nameIter.value();
                break;
            }
        }

        // Get a translated description if possible
        QLabel *descLabel = new QLabel( vbox );
        QString desc;
        if ( parsedHook.contains( "Description-" + language ) )
            desc = parsedHook.value( "Description-" + language );
        else
        {
            QMap<QString, QString>::const_iterator descIter = parsedHook.constFind("Description");
            while (descIter != parsedHook.end() && descIter.key() == "Description")
            {
                desc = descIter.value();
                break;
            }
        }

        descLabel->setWordWrap( true );
        descLabel->setText( desc );

        QString command;
        QMap<QString, QString>::const_iterator commandIter = parsedHook.constFind("Command");
        while (commandIter != parsedHook.end() && commandIter.key() == "Command")
        {
            command = commandIter.value();
            break;
        }

        bool terminal = false;
        QMap<QString, QString>::const_iterator terminalIter = parsedHook.constFind("Terminal");
        while (terminalIter != parsedHook.end() && terminalIter.key() == "Command")
        {
            QString terminalValue = commandIter.value();
            if ( terminalValue == "True" )
                terminal = true;
            break;
        }

        if ( !command.isEmpty() )
        {
            QPushButton *runButton = new QPushButton( KIcon( "system-run" ), i18n( "Run this action now" ), vbox );
            // FIXME: How to pass command and terminal to the slot properly? This doesn't work.
            connect( runButton, SIGNAL( clicked() ), this, SLOT( runHookCommand( command, terminal ) ) );
        }

        KPageWidgetItem *page = new KPageWidgetItem( vbox, name );
        page->setIcon( KIcon( "help-hint" ) );

        dialog->addPage( page );
    }

    dialog->show();
}

void UpdateHelperNotifier::runHookCommand( QString command, bool terminal )
{

}

void UpdateHelperNotifier::cleanUpDialog()
{
    dialog->deleteLater();
    dialog = 0;
}

#include "updatehelpernotifier.moc"
