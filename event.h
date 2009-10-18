/***************************************************************************
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
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
        void show( QPixmap icon, QString text, QStringList actions );

        const QString name;
        bool hidden;
        KNotification *notify;

        virtual ~Event();

    public slots:
        void run();

    private slots:
        void ignore();
        void hide();
        void notifyClosed();

    private:
        bool active;

        QString cfgstring;

        bool readHidden();
        void writeHidden( bool value );
};

#endif
