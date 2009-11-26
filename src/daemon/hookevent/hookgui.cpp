/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
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

#include "hookgui.h"

// Qt includes
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

// KDE includes
#include <KGlobal>
#include <KProcess>
#include <KIcon>
#include <KLocale>
#include <KToolInvocation>
#include <KVBox>

HookGui::HookGui(QObject* parent, QList<Hook*> hooks)
        : QObject(parent)
        , m_hooks(hooks)
        , m_dialog(0)
{
    m_dialog = new KPageDialog;
    m_dialog->setCaption(i18n("Update Information"));
    m_dialog->setWindowIcon(KIcon("help-hint"));
    m_dialog->setButtons(KDialog::Ok);
    connect(m_dialog, SIGNAL(okClicked()), SLOT(cleanUpDialog()));

    // Take the parsed upgrade hook(s) and put them in pages
    const QString language =  KGlobal::locale()->language();
    QList<Hook*>::iterator i;
    for (i = hooks.begin(); i !=  hooks.end(); ++i) {
        Hook *hook = *i;

        KVBox *vbox = new KVBox();

        QString desc = hook->getField("Description", language);
        QLabel *descLabel = new QLabel(vbox);
        descLabel->setWordWrap(true);
        descLabel->setText(desc);

        if (!hook->getField("Command").isEmpty()) {
            QPushButton *runButton = new QPushButton(KIcon("system-run"), i18n("Run this action now"), vbox);
            connect(runButton, SIGNAL(clicked()), hook, SLOT(runCommand()));
        }

        QString name = hook->getField("Name", language);
        KPageWidgetItem *page = new KPageWidgetItem(vbox, name);
        page->setIcon(KIcon("help-hint"));

        m_dialog->addPage(page);
    }

    m_dialog->show();
}

HookGui::~HookGui()
{
    delete m_dialog;
}

void HookGui::cleanUpDialog()
{
    m_dialog->deleteLater();
    m_dialog = 0;
}

#include "hookgui.moc"
