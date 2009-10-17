/****************************************************************************

 Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>

 Distributed under the terms of the GNU General Public License version 3.

****************************************************************************/

#ifndef HOOKEVENT_H
#define HOOKEVENT_H

#include "event.h"

// KDE includes
#include <KPageDialog>

class HookEvent : public Event
{
    Q_OBJECT
    public:
        HookEvent( QObject* parent, QString name );
        void show();

        virtual ~HookEvent();

    private slots:
        void run();
        void cleanUpDialog();

    private:
        KPageDialog* dialog;
};

#endif
