/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

#ifndef EVENT_H
#define EVENT_H

#include <QObject>

#include <KDebug>
#include <KNotification>

// for implementing classes
#include <KLocale>
#include <KIcon>

class Event
: public QObject
{
    Q_OBJECT
    public:
        Event( QObject* parent, QString name );
        void show(QPixmap icon, QString text, QStringList actions);

        const QString name;
        bool hidden;
        KNotification *notify;

        virtual ~Event();

    public slots:
        void run();

    private slots:
        void ignore();
        void hide();

    private:
        bool active;

        QString cfgstring;

        bool readHidden();
        void writeHidden(bool value);
};

#endif
