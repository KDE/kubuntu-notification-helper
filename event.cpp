/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

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
    KConfig cfg("notificationhelper");
    KConfigGroup notifyGroup( &cfg, "Event" );
    return notifyGroup.readEntry(cfgstring, false );

}

void Event::writeHidden(bool value)
{
    KConfig cfg("notificationhelper");
    KConfigGroup notifyGroup( &cfg, "Event" );
    kDebug() << cfgstring << " " << value;
    notifyGroup.writeEntry( cfgstring, value );
    notifyGroup.config()->sync();
}

void Event::show(QPixmap icon, QString text, QStringList actions)
{
    if(active || hidden)
        return;

    active = true;
kDebug()<<name;
    KNotification *notify = new KNotification(name, 0, KNotification::Persistent);
    notify->setComponentData(KComponentData("notificationhelper"));

    notify->setPixmap( icon );
    notify->setText( text );
    notify->setActions( actions );

    connect( notify, SIGNAL( action1Activated() ), this, SLOT( run() ) );
    connect( notify, SIGNAL( action2Activated() ), this, SLOT( ignore() ) );
    connect( notify, SIGNAL( action3Activated() ), this, SLOT( hide() ) );

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
    writeHidden(true);
    hidden = true;
//     notify->deleteLater();
}

#include "event.moc"
