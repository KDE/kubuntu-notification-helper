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

#include "notificationhelper.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>

// KDE
#include <KDirWatch>
#include <KDebug>

// Lower-level include for sleep
#include <unistd.h>


NotificationHelper::NotificationHelper( QObject* parent )
    : QObject( parent )
    , aEvent(0)
    , hEvent(0)
    , rEvent(0)
{
    aEvent = new ApportEvent(this, "Apport");
    hEvent = new HookEvent(this, "Hook");
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
        connect( apportDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( apportEvent() ) );

        // Force check since we just started up and there might have been crashes on reboot
        QTimer::singleShot(5000, this, SLOT(apportEvent()));
    }

    if ( !hEvent->hidden )
    {
        KDirWatch *hooksDirWatch = new KDirWatch( this );
        hooksDirWatch->addDir( "/var/lib/update-notifier/user.d/" );
        connect( hooksDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( hookEvent() ) );
    }
}

NotificationHelper::~NotificationHelper()
{
    delete aEvent;
    delete hEvent;
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

void NotificationHelper::hookEvent()
{
    hEvent->show();
}

#include "notificationhelper.moc"
