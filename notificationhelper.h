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

#ifndef UPDATEHELPERNOTIFIER_H
#define UPDATEHELPERNOTIFIER_H

// Qt includes
#include <QTimer>
#include <QAction>

// KDE includes
#include <KNotification>
#include <KPageDialog>

class NotificationHelper
: public QObject
{
    Q_OBJECT
    public:
        NotificationHelper( QObject *parent = 0 );

        virtual ~NotificationHelper();

    private slots:
        void aptDirectoryChanged();
        void apportDirectoryChanged();
        void hooksDirectoryChanged();
        void restartActivated();
        void disableRestartNotification();
        void runApport();
        void apportNotifyClosed();
        void hooksActivated();
        void runHookCommand( QString command, bool terminal );
        void cleanUpDialog();

    private:
        bool apportNotifyShowing;
        bool showRestartNotification;
        QMap< QString, QMap< QString, QString > > parsedHookMap;
        KPageDialog* dialog;

        int checkApport( bool system );
        QMap< QString, QString > processUpgradeHook( QString fileName );

};

#endif
