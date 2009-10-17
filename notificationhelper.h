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

#ifndef UPDATEHELPERNOTIFIER_H
#define UPDATEHELPERNOTIFIER_H

#include "rebootevent.h"
#include "apportevent.h"
// #include "hookevent.h"

// Qt includes
#include <QTimer>
#include <QAction>

class NotificationHelper
: public QObject
{
    Q_OBJECT
    public:
        NotificationHelper( QObject *parent = 0 );

        virtual ~NotificationHelper();

    private slots:
        void apportEvent();
//         void hookEvent();
        void rebootEvent();

//         void hooksDirectoryChanged();
//         void runApport();
//         void hooksActivated();
//         void runHookCommand( QString command, bool terminal );

    private:
//         QMap< QString, QMap< QString, QString > > parsedHookMap;
        ApportEvent* aEvent;
//         HookEvent* hEvent;
        RebootEvent* rEvent;

//         int checkApport( bool system );
//         QMap< QString, QString > processUpgradeHook( QString fileName );
};

#endif
