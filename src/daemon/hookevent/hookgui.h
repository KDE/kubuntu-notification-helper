/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
 *   Copyright © 2009 Amichai Rothman <amichai2@amichais.net>              *
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

#ifndef HOOKGUI_H
#define HOOKGUI_H

#include "hook.h"

class Hook;

class KPageDialog;
class KPageWidgetItem;

class HookGui : public QObject
{
    Q_OBJECT
public:
    HookGui(QObject* parent);

    virtual ~HookGui();

public slots:
    void showDialog(QList<Hook*> hooks);

private slots:
    void createDialog();
    void updateDialog(QList<Hook*> hooks);
    void closeDialog();
    void runCommand(QObject *obj);

private:
    KPageDialog* m_dialog;
    QList<KPageWidgetItem *> m_pages;
};

#endif
