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
