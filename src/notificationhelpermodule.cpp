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

#include "notificationhelpermodule.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>

// KDE
#include <KAboutData>
#include <KDirWatch>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY(NotificationHelperModuleFactory,
                 registerPlugin<NotificationHelperModule>();
    )
K_EXPORT_PLUGIN(NotificationHelperModuleFactory("NotificationHelper"))


NotificationHelperModule::NotificationHelperModule(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
    , aEvent(0)
    , hEvent(0)
    , rEvent(0)
{
    // FIXME: As far as I can tell, localization doesn't work
    KGlobal::locale()->insertCatalog("notificationhelper");

    KAboutData aboutData("notificationhelper", "notificationhelper", ki18n("Kubuntu Notification Helper"),
                         "0.3", ki18n("A Notification Helper for kubuntu"),
                         KAboutData::License_GPL, ki18n("(C) 2009 Jonathan Thomas, (C) 2009 Harald Sitter"),
                         KLocalizedString(), "http://kubuntu.org");

    aEvent = new ApportEvent( this, "Apport" );
    hEvent = new HookEvent( this, "Hook" );
    rEvent = new RebootEvent( this, "Restart" );

    if ( !rEvent->hidden )
    {
        KDirWatch *stampDirWatch = new KDirWatch( this );
        stampDirWatch->addFile( "/var/lib/update-notifier/dpkg-run-stamp" );
        connect( stampDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( rebootEvent() ) );
        rebootEvent();
    }


   if ( !aEvent->hidden && ( QFile::exists( "/usr/share/apport/apport-kde" ) || QFile::exists( "/usr/share/apport/apport-gtk" ) ) )
    {
        KDirWatch *apportDirWatch =  new KDirWatch( this );
        apportDirWatch->addDir( "/var/crash/" );
        connect( apportDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( apportEvent() ) );

        // Force check since we just started up and there might have been crashes on reboot
        QTimer::singleShot( 5000, this, SLOT( apportEvent() ) );
    }

    if ( !hEvent->hidden )
    {
        KDirWatch *hooksDirWatch = new KDirWatch( this );
        hooksDirWatch->addDir( "/var/lib/update-notifier/user.d/" );
        connect( hooksDirWatch, SIGNAL( dirty( const QString & ) ), this, SLOT( hookEvent() ) );
    }
}

NotificationHelperModule::~NotificationHelperModule()
{
    delete aEvent;
    delete hEvent;
    delete rEvent;
}

void NotificationHelperModule::rebootEvent()
{
    if ( !QFile::exists( "/var/run/reboot-required" ) )
        return;
    rEvent->show();
}

void NotificationHelperModule::apportEvent()
{
    // We could be too fast for apport, so wait a bit before showing the notification
    QTimer::singleShot( 2000, aEvent, SLOT( show() ) );
}

void NotificationHelperModule::hookEvent()
{
    hEvent->show();
}

#include "notificationhelpermodule.moc"
