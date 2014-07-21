/***************************************************************************
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "event.h"

#include <QIcon>
#include <QMenu>
#include <QStringBuilder>

#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <KNotification>
#include <KStatusNotifierItem>

Event::Event(QObject* parent, const QString &name)
        : QObject(parent)
        , m_name(name)
        , m_hidden(false)
        , m_active(false)
        , m_notifierItem(0)
{
    m_hiddenCfgString = QString("hide" % m_name % "Notifier");
    m_hidden = readHiddenConfig();
    readNotifyConfig();
}

Event::~Event()
{
}

bool Event::readHiddenConfig()
{
    KConfig cfg("notificationhelper");
    KConfigGroup notifyGroup(&cfg, "Event");
    return notifyGroup.readEntry(m_hiddenCfgString, false);
}

void Event::writeHiddenConfig(bool value)
{
    KConfig cfg("notificationhelper");
    KConfigGroup notifyGroup(&cfg, "Event");
    notifyGroup.writeEntry(m_hiddenCfgString, value);
    notifyGroup.config()->sync();
}

void Event::readNotifyConfig()
{
    KConfig cfg("notificationhelper");
    KConfigGroup notifyTypeGroup(&cfg, "NotificationType");
    QString notifyType = notifyTypeGroup.readEntry("NotifyType", "Combo");

    if (notifyType == "Combo") {
        m_useKNotify = true;
        m_useTrayIcon = true;
    } else if (notifyType == "TrayOnly") {
        m_useKNotify = false;
        m_useTrayIcon = true;
    } else { // KNotifyOnly
        m_useKNotify = true;
        m_useTrayIcon = false;
    }
}

bool Event::isHidden() const
{
    return m_hidden;
}

void Event::show(const QString &icon, const QString &text, const QStringList &actions)
{
    if (m_active || m_hidden) {
        return;
    }

    // Only manually compose a notification if notifications are enabled AND
    // we don't have a tray icon, otherwise the tray icon will issue the notification.
    if (m_useKNotify && !m_useTrayIcon) {
        KNotification::NotificationFlag flag;

        if (!m_useTrayIcon) {
            // Tray icon not in use, so be persistent
            flag = KNotification::Persistent;
        }

        m_active = true;
        KNotification *notify = new KNotification(m_name, 0, flag);
        notify->setComponentName("notificationhelper");
        notify->setPixmap(QIcon::fromTheme(icon).pixmap(NOTIFICATION_ICON_SIZE));
        notify->setText(text);

        // Tray icon not in use to handle actions
        notify->setActions(actions);
        connect(notify, SIGNAL(action1Activated()), this, SLOT(run()));
        connect(notify, SIGNAL(action2Activated()), this, SLOT(ignore()));
        connect(notify, SIGNAL(action3Activated()), this, SLOT(hide()));
        connect(notify, SIGNAL(closed()), this, SLOT(notifyClosed()));

        notify->sendEvent();
    }

    if (m_useTrayIcon && !m_notifierItem) { // Only create item when there is none.
        m_notifierItem = new KStatusNotifierItem(this);
        m_notifierItem->setIconByName(icon);
        m_notifierItem->setToolTipIconByName(icon);
        m_notifierItem->setToolTipTitle(i18n("System Notification Helper"));
        m_notifierItem->setToolTipSubTitle(text);
        m_notifierItem->setStatus(KStatusNotifierItem::Active);
        m_notifierItem->setCategory(KStatusNotifierItem::SystemServices);
        m_notifierItem->setStandardActionsEnabled(false);

        QMenu *contextMenu = new QMenu(0);
        // FIXME: addtitle does a whole pile of madness, I am not sure that should
        //        be reused *at all* as the lack of frameworks continuity will
        //        make for shitty UX
//         contextMenu->addTitle(QIcon::fromTheme("applications-system"), i18n("System Notification Helper"));

        QAction *runAction = contextMenu->addAction(actions.at(0));
        // FIXME: DBusmenu doesn't support pixmap icons yet. Change function to take QIcon::fromTheme
        runAction->setIcon(QIcon::fromTheme(icon));
        connect(runAction, SIGNAL(triggered()), this, SLOT(run()));
        contextMenu->addAction(runAction);

        QAction *ignoreForeverAction = contextMenu->addAction(actions.at(2));
        connect(ignoreForeverAction, SIGNAL(triggered()), this, SLOT(hide()));
        contextMenu->addAction(ignoreForeverAction);

        contextMenu->addSeparator();

        QAction *hideAction = contextMenu->addAction(i18n("Hide"));
        hideAction->setIcon(QIcon::fromTheme("application-exit"));
        connect(hideAction, SIGNAL(triggered()), this, SLOT(ignore()));
        contextMenu->addAction(hideAction);

        m_notifierItem->setContextMenu(contextMenu);
        m_notifierItem->setAssociatedWidget(NULL);

        connect(m_notifierItem, SIGNAL(activateRequested(bool, const QPoint &)), this, SLOT(run()));
    }

    // Ask the KSNI to show a notification whenver show() is called.
    if (m_useTrayIcon && m_notifierItem) {
        m_notifierItem->showMessage(i18nc("notification title", "System Notification Helper"),
                                    text, icon);
    }
}

void Event::run()
{
    delete m_notifierItem;
    m_notifierItem = 0;
    notifyClosed();
}

void Event::ignore()
{
    m_notifierItem->deleteLater();
    m_notifierItem = 0;
    notifyClosed();
}

void Event::hide()
{
    notifyClosed();
    writeHiddenConfig(true);
    m_hidden = true;
}

void Event::notifyClosed()
{
    m_active = false;
}

void Event::reloadConfig()
{
    m_hidden = readHiddenConfig();
}
