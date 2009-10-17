/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

#ifndef REBOOTEVENT_H
#define REBOOTEVENT_H

#include "event.h"

class RebootEvent : public Event
{
    Q_OBJECT
    public:
        RebootEvent( QObject* parent, QString name );
        void show();

        virtual ~RebootEvent();

    private slots:
        void run();
};

#endif
