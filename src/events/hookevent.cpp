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

#include "hookevent.h"

// Qt includes
#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

// KDE includes
#include <KGlobal>
#include <KProcess>
#include <KIcon>
#include <KLocale>
#include <KVBox>

// Lower-level includes for dirent
#include <dirent.h>
#include <kde_file.h>

HookEvent::HookEvent( QObject* parent, QString name)
    : Event(parent, name)
    , dialog(0)
    , parsedHookMap()
    , command(0)
    , terminal(false)
{}

HookEvent::~HookEvent()
{
    delete dialog;
}

void HookEvent::show()
{
    QStringList fileList;

    DIR *dp = opendir( QFile::encodeName( "/var/lib/update-notifier/user.d/" ) );
    KDE_struct_dirent *ep;

    while( ( ep = KDE_readdir( dp ) ) != 0L )
    {
        QString fn( QFile::decodeName( ep->d_name ) );
        if (fn == "." || fn == ".." || fn.at(fn.length() - 1) == '~')
            continue;

        fileList << QFile::decodeName(ep->d_name);
    }

    foreach ( const QString &fileName, fileList ) {
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
        QPixmap icon = KIcon( "help-hint" ).pixmap( 48, 48 );
        QString text(i18nc( "Notification when an upgrade requires the user to do something", "Software upgrade notifications are available" ) );
        QStringList actions;
        actions << i18nc( "Opens a dialog with more details", "Details" );
        actions << i18nc( "User declines an action", "Ignore" );
        actions << i18nc( "User indicates he never wants to see this notification again", "Never show again" );
        Event::show(icon,text,actions);
    }
    else
    {
        // TODO: Destroy self
    }
}

QMap<QString, QString> HookEvent::processUpgradeHook( QString fileName )
{
    QMap< QString, QString > fileInfo;
    QMap< QString, QString > emptyMap;

    // Open the upgrade hook file
    QFile hookFile("/var/lib/update-notifier/user.d/" + fileName );

    /* Parsing magic. (https://wiki.kubuntu.org/InteractiveUpgradeHooks)
    Read the hook to a QString, split it up at each line. Then if a colon
    is in the line, split it into two fields, they key and the value.
    the line. Make a key/value pair in our fileInfo map. If the line beings
    with a space, append it to the value of the most recently added key.
    (As seen in the case of multi-line upgrade hook description fields) */
    if ( hookFile.open( QFile::ReadOnly ) )
    {
        QTextStream stream( &hookFile );
        QString streamAllString = stream.readAll();
        QStringList streamList = streamAllString.split( '\n' );
        foreach ( const QString &streamLine, streamList )
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

void HookEvent::run()
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

        QString language =  KGlobal::locale()->language();

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

        QMap<QString, QString>::const_iterator commandIter = parsedHook.constFind("Command");
        while (commandIter != parsedHook.end() && commandIter.key() == "Command")
        {
            command = commandIter.value();
            break;
        }

        QMap<QString, QString>::const_iterator terminalIter = parsedHook.constFind("Terminal");
        while (terminalIter != parsedHook.end() && terminalIter.key() == "Terminal")
        {
            QString terminalValue = terminalIter.value();
            if ( terminalValue == "True" )
                terminal = true;
            break;
        }

        if ( !command.isEmpty() )
        {
            QPushButton *runButton = new QPushButton( KIcon( "system-run" ), i18n( "Run this action now" ), vbox );
            // FIXME: How to pass command and terminal to the slot properly? This doesn't work.
            connect( runButton, SIGNAL( clicked() ), this, SLOT( runHookCommand() ) );
        }

        KPageWidgetItem *page = new KPageWidgetItem( vbox, name );
        page->setIcon( KIcon( "help-hint" ) );

        dialog->addPage( page );
    }

    dialog->show();
    Event::run();
}

void HookEvent::runHookCommand()
{
    if ( terminal )
    {
        // if command is quoted, invokeTerminal will refuse to interpret it properly
        if ( command.startsWith( '\"' ) && command.endsWith( '\"' ) )
        {
        // FIXME: the fudge? I have no clue what this does in python, no chance of me being able to port it
//             self.command = self.command[1:][:-1]
//             KToolInvocation.invokeTerminal(self.command)
        }
    }
    else
    {
        KProcess *process = new KProcess();
        process->setShellCommand( command );
        process->startDetached();
        kDebug() << "invoking upgrade hook command";
    }

    command.clear();
    terminal = false;
}

void HookEvent::cleanUpDialog()
{
    dialog->deleteLater();
    dialog = 0;
}

#include "hookevent.moc"
