/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

#ifndef APPORTEVENT_H
#define APPORTEVENT_H

#include "event.h"

class ApportEvent : public Event
{
    Q_OBJECT
    public:
        ApportEvent( QObject* parent, QString name );
        void show();

        virtual ~ApportEvent();

    private slots:
        void run();
//         int checkApport();
};

#endif
