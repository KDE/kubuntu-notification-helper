/***************************************************************************
 *   Copyright (C) 2009 by Jonathan Thomas <echidnaman@kubuntu.org>        *
 *                                                                         *
 * This is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License version 3   *
 * only, as published by the Free Software Foundation.                     *
 *                                                                         *
 * This is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Lesser General Public License version 3 for more details            *
 * (a copy is included in the LICENSE file that accompanied this code).    *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public License*
 * version 3 along with Update Notifier Helper.  If not, see               *
 * <http://www.gnu.org/licenses/> for a copy of the LGPLv3 License.        *
 ***************************************************************************/

#ifndef _UPDATEHELPERNOTIFIER_H_
#define _UPDATEHELPERNOTIFIER_H_

// Qt includes
#include <QTimer>
#include <QAction>

// KDE includes
#include <KDirWatch>
#include <KNotification>


class UpdateHelperNotifier
: public QObject
{
    Q_OBJECT
    public:
        UpdateHelperNotifier( QObject *parent = 0 );

        virtual ~UpdateHelperNotifier();

    private slots:
        void aptDirectoryChanged();
        void apportDirectoryChanged();
        void restartActivated();
        void disableRestartNotification();
        void runApport();
        void apportNotifyClosed();

    private:
        KDirWatch *dirWatch;
        KDirWatch *apportDirWatch;

        bool apportNotifyShowing;
        bool showRestartNotification;

        int checkApport( bool system );

};

#endif
