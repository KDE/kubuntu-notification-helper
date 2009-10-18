/***************************************************************************
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

#include "event.h"

#include <KConfigGroup>

Event::Event( QObject* parent, QString name)
    : QObject( parent )
    , name(name)
    , hidden(false)
    , active(false)
{
    cfgstring = "hide" + name + "Notifier";
    hidden = readHidden();
}

Event::~Event()
{
    kDebug() << "close";
//     notify->deleteLater();
    kDebug();
}

bool Event::readHidden()
{
    KConfig cfg( "notificationhelper" );
    KConfigGroup notifyGroup( &cfg, "Event" );
    return notifyGroup.readEntry( cfgstring, false );

}

void Event::writeHidden(bool value)
{
    KConfig cfg( "notificationhelper" );
    KConfigGroup notifyGroup( &cfg, "Event" );
    kDebug() << cfgstring << " " << value;
    notifyGroup.writeEntry( cfgstring, value );
    notifyGroup.config()->sync();
}

void Event::show(QPixmap icon, QString text, QStringList actions)
{
    if( active || hidden )
        return;

    active = true;
    kDebug() << name;
    KNotification *notify = new KNotification( name, 0, KNotification::Persistent );
    notify->setComponentData( KComponentData( "notificationhelper" ) );

    notify->setPixmap( icon );
    notify->setText( text );
    notify->setActions( actions );

    connect( notify, SIGNAL( action1Activated() ), this, SLOT( run() ) );
    connect( notify, SIGNAL( action2Activated() ), this, SLOT( ignore() ) );
    connect( notify, SIGNAL( action3Activated() ), this, SLOT( hide() ) );

    connect( notify, SIGNAL( closed() ), this, SLOT( notifyClosed() ) );

    notify->sendEvent();
}

void Event::run()
{
//     notify->deleteLater();
}

void Event::ignore()
{
//     notify->deleteLater();
}

void Event::hide()
{
    writeHidden( true );
    hidden = true;
//     notify->deleteLater();
}

void Event::notifyClosed()
{
    active = false;
}

#include "event.moc"
