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

#include "notificationhelper.h"

// Qt
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

// KDE
#include <KDirWatch>
#include <KDebug>
#include <KGlobal>
#include <KLocale>

// Lower level includes for sleep, dirent
#include <unistd.h>
#include <dirent.h>
#include <kde_file.h>


NotificationHelper::NotificationHelper( QObject* parent )
    : QObject( parent )
//     , parsedHookMap()
    , aEvent(0)
//     , hEvent(0)
    , rEvent(0)
{
    aEvent = new ApportEvent(this, "Restart");
//     hEvent = new HookEvent(this, "Hook");
    rEvent = new RebootEvent(this, "Restart");

    if ( !rEvent->hidden )
    {
        KDirWatch *stampDirWatch = new KDirWatch( this );
        stampDirWatch->addFile( "/var/lib/update-notifier/dpkg-run-stamp" );
        connect( stampDirWatch, SIGNAL( created( const QString & ) ), this, SLOT( rebootEvent() ) );
        rebootEvent();
    }


   if ( !aEvent->hidden && (QFile::exists( "/usr/share/apport/apport-kde" ) || QFile::exists( "/usr/share/apport/apport-gtk" )) )
    {
        KDirWatch *apportDirWatch =  new KDirWatch( this );
        apportDirWatch->addDir( "/var/crash/" );
        apportNotifyShowing = false;
        connect( apportDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( apportEvent() ) );

        // Force check since we just started up and there might have been crashes on reboot
        QTimer::singleShot(5000, this, SLOT(apportEvent()));
    }

//     if ( !hEvent->hidden )
//     {
//         KDirWatch *hooksDirWatch = new KDirWatch( this );
//         hooksDirWatch->addDir( "/var/lib/update-notifier/user.d/" );
//         connect( hooksDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( hooksDirectoryChanged() ) );
//     }
}

NotificationHelper::~NotificationHelper()
{
    delete aEvent;
//     delete hEvent;
    delete rEvent;
}

void NotificationHelper::rebootEvent()
{
    if ( !QFile::exists( "/var/run/reboot-required" ) )
        return;
    rEvent->show();
}

void NotificationHelper::apportEvent()
{
    // We could be too fast for apport,  so wait a bit before showing the notification
    sleep ( 2 );
    aEvent->show();
}

// void NotificationHelper::hookEvent()
// {
//     HookEvent *hEvent = new HookEvent(this, "Hook");
//     hEvent->show();
// }

// void NotificationHelper::hooksDirectoryChanged()
// {
//     QStringList fileList;
// 
//     DIR *dp = opendir( QFile::encodeName( "/var/lib/update-notifier/user.d/" ) );
//     KDE_struct_dirent *ep;
// 
//     while( ( ep = KDE_readdir( dp ) ) != 0L )
//     {
//         QString fn( QFile::decodeName( ep->d_name ) );
//         if (fn == "." || fn == ".." || fn.at(fn.length() - 1) == '~')
//             continue;
// 
//         fileList << QFile::decodeName(ep->d_name);
//     }
// 
//     foreach ( const QString &fileName, fileList ) {
//         QMap< QString, QString > fileResult = processUpgradeHook( fileName );
//         // if not empty, add parsed hook to the list of parsed hooks
//         if ( !fileResult.isEmpty() )
//         {
//             fileResult["fileName"] = fileName;
//             parsedHookMap[fileName] = fileResult;
//         }
//     }
// 
//     if ( !parsedHookMap.isEmpty() )
//     {
//         hookEvent();
//     }
// }


#include "notificationhelper.moc"
