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

#ifndef MODULE_H
#define MODULE_H

#include <KDEDModule>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include "notificationhelper.h"

class NotificationHelperModule
    : public KDEDModule
    {
    Q_OBJECT
    public:
        NotificationHelperModule(QObject* parent, const QList<QVariant>&);
    private:
        NotificationHelper notifier;
    };

#endif
