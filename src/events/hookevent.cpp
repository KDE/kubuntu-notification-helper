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
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

// KDE includes
#include <KDebug>
#include <KProcess>

// Own includes
#include "hookgui.h"

HookEvent::HookEvent( QObject* parent, QString name)
    : Event(parent, name)
    , parsedHookMap()
{}

HookEvent::~HookEvent()
{}

void HookEvent::show()
{
    QDir hookDir( "/var/lib/update-notifier/user.d/" );

    QStringList fileList;
    fileList << hookDir.entryList( QDir::Files );

    foreach ( const QString &fileName, fileList ) {
        QMap< QString, QString > fileResult = processUpgradeHook( fileName );
        // if not empty, add parsed hook to the list of parsed hooks
        if ( !fileResult.isEmpty() )
            parsedHookMap[fileName] = fileResult;
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
            // Handle empty newline(s) at the end of files
            else if ( streamLine.isEmpty() )
            {
                continue;
            }
            // Not an upgrade hook or malformed
            else
            {
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

    if ( fileInfo.contains( "DisplayIf" ) )
    {
        KProcess *displayIfProcess = new KProcess();
        displayIfProcess->setProgram( fileInfo.value( "DisplayIf" ) );

        int programResult = displayIfProcess->execute();
        if ( programResult != 0 )
            return emptyMap;
    }

    return fileInfo;
}

void HookEvent::run()
{
    HookGui* gui = new HookGui( this, parsedHookMap );
    kDebug() << "running the GUI class";
    Event::run();
}

#include "hookevent.moc"
