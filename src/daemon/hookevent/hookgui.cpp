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

#include "hookgui.h"

// Qt includes
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QSignalMapper>

// KDE includes
#include <KGlobal>
#include <KIcon>
#include <KLocale>
#include <KVBox>

HookGui::HookGui(QObject* parent)
        : QObject(parent)
        , m_dialog(0)
{}

void HookGui::showDialog(QList<Hook*> hooks)
{
    if (!m_dialog)
        createDialog();
    updateDialog(hooks);
}

void HookGui::createDialog()
{
    m_dialog = new KPageDialog;
    m_dialog->setCaption(i18n("Update Information"));
    m_dialog->setWindowIcon(KIcon("help-hint"));
    m_dialog->setButtons(KDialog::Ok);
    connect(m_dialog, SIGNAL(okClicked()), SLOT(closeDialog()));
}

void HookGui::updateDialog(QList<Hook*> hooks)
{
    if (!m_pages.isEmpty()) {
        m_dialog->hide();
        // remove old pages
        foreach (KPageWidgetItem *page, m_pages) {
            m_dialog->removePage(page);
        }
        m_pages.clear();
    }

    // Take the parsed upgrade hook(s) and put them in pages
    const QString language =  KGlobal::locale()->language();
    QSignalMapper *signalMapper = new QSignalMapper(m_dialog);
    QList<Hook*>::iterator i;
    for (i = hooks.begin(); i !=  hooks.end(); ++i) {
        Hook *hook = *i;

        KVBox *vbox = new KVBox();
        QString name = hook->getField("Name", language);
        KPageWidgetItem *page = new KPageWidgetItem(vbox, name);
        page->setIcon(KIcon("help-hint"));
        page->setProperty("hook", qVariantFromValue((QObject *)hook));

        QString desc = hook->getField("Description", language);
        QLabel *descLabel = new QLabel(vbox);
        descLabel->setWordWrap(true);
        descLabel->setText(desc);

        if (!hook->getField("Command").isEmpty()) {
            QString label = hook->getField("ButtonText", language);
            if (label.isEmpty())
                label = i18n("Run this action now");
            QPushButton *runButton = new QPushButton(KIcon("system-run"), label, vbox);
            runButton->setObjectName("runButton");
            signalMapper->setMapping(runButton, page);
            connect(runButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        }

        m_dialog->addPage(page);
        m_pages << page;
    }

    connect(signalMapper, SIGNAL(mapped(QObject *)),
            this, SLOT(runCommand(QObject *)));

    m_dialog->show();
    m_dialog->raise();
    m_dialog->activateWindow();
}

HookGui::~HookGui()
{
    delete m_dialog;
}

void HookGui::runCommand(QObject *obj) {
    KPageWidgetItem *page = (KPageWidgetItem *)obj;
    Hook *hook = (Hook *)qvariant_cast<QObject *>(page->property("hook"));
    QWidget *widget = page->widget();

    QPushButton *runButton = widget->findChild<QPushButton *>("runButton");
    runButton->setEnabled(false);

    hook->runCommand();
    hook->setFinished();
}

void HookGui::closeDialog()
{
    m_dialog->deleteLater();
    m_dialog = 0;
    m_pages.clear();
}

#include "hookgui.moc"
