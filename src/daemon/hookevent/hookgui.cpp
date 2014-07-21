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
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

// KDE includes
#include <KLocalizedString>
#include <KPageDialog>
#include <KWindowSystem>

HookGui::HookGui(QObject* parent)
        : QObject(parent)
        , m_dialog(0)
{}

void HookGui::showDialog(QList<Hook*> hooks)
{
    if (!m_dialog) {
        createDialog();
    }
    updateDialog(hooks);
}

void HookGui::createDialog()
{
    m_dialog = new KPageDialog;
    m_dialog->setWindowTitle(i18n("Update Information"));
    m_dialog->setWindowIcon(QIcon::fromTheme("help-hint"));
    m_dialog->setStandardButtons(QDialogButtonBox::Close);
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
    QSignalMapper *signalMapper = new QSignalMapper(m_dialog);
    foreach(const Hook *hook, hooks) {
        QWidget *content = new QWidget();
        content->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        QVBoxLayout *layout = new QVBoxLayout(content);
        layout->setMargin(0);

        QString name = hook->getField("Name");
        KPageWidgetItem *page = new KPageWidgetItem(content, name);
        page->setIcon(QIcon::fromTheme("help-hint"));
        page->setProperty("hook", qVariantFromValue((QObject *)hook));

        QString desc = hook->getField("Description");
        QLabel *descLabel = new QLabel(content);
        descLabel->setWordWrap(true);
        descLabel->setText(desc);
        layout->addWidget(descLabel);

        if (!hook->getField("Command").isEmpty()) {
#warning fixme do we need this?
//             layout->addSpacing(2 * KDialog::spacingHint());
            QString label = hook->getField("ButtonText");
            if (label.isEmpty())
                label = i18n("Run this action now");
            QPushButton *runButton = new QPushButton(QIcon::fromTheme("system-run"), label, content);
            runButton->setFixedHeight(runButton->sizeHint().height() * 2);
            runButton->setObjectName("runButton");

            QHBoxLayout *buttonLayout = new QHBoxLayout();
            buttonLayout->addStretch();
            buttonLayout->addWidget(runButton);
            buttonLayout->addStretch();
            layout->addItem(buttonLayout);

            signalMapper->setMapping(runButton, page);
            connect(runButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        }

        m_dialog->addPage(page);
        m_pages << page;
    }

    connect(signalMapper, SIGNAL(mapped(QObject *)),
            this, SLOT(runCommand(QObject *)));

    m_dialog->show();
    KWindowSystem::forceActiveWindow(m_dialog->winId());
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
