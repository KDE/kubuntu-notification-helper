/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
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

#include "installgui.h"

// Qt includes
#include <QLabel>
#include <QVBoxLayout>

// KDE includes
#include <KLocalizedString>
#include <KNotification>
#include <KToolInvocation>

InstallGui::InstallGui(QObject* parent, const QString application, const QMap<QString, QString> packageList)
        : QObject(parent)
        , dialog(0)
        , applicationName(0)
        , toInstallList(0)
{
    toInstallList.clear();
    applicationName = application;

    dialog = new KDialog;
    dialog->setWindowIcon(KIcon("download"));
    dialog->setCaption(i18n("Install Packages"));
    dialog->setButtons(KDialog::Ok | KDialog::Cancel);
    dialog->setButtonText(KDialog::Ok, i18n("Install Selected"));
    connect(dialog, SIGNAL(okClicked()), SLOT(runPackageInstall()));
    connect(dialog, SIGNAL(okClicked()), SLOT(cleanUpDialog()));

    QWidget* widget = new QWidget(dialog);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    widget->setLayout(layout);

    QLabel* label = new QLabel(widget);
    label->setWordWrap(true);
    label->setText(i18n("For extra multimedia functionality, select restricted packages to be installed."));
    layout->addWidget(label);

    QListWidget* listWidget = new QListWidget(widget);
    connect(listWidget, SIGNAL(itemChanged(QListWidgetItem *)), SLOT(packageToggled(QListWidgetItem *)));
    layout->addWidget(listWidget);

    QMap<QString, QString>::const_iterator nameIter = packageList.constBegin();
    while (nameIter != packageList.end()) {
        QListWidgetItem* item = new QListWidgetItem(nameIter.value());
        item->setToolTip(nameIter.key());
        toInstallList << nameIter.key();
        item->setCheckState(Qt::Checked);
        listWidget->addItem(item);
        ++nameIter;
    }

    dialog->setMainWidget(widget);
    dialog->show();
}

InstallGui::~InstallGui()
{
    delete dialog;
}

void InstallGui::packageToggled(QListWidgetItem *item)
{
    if (item->checkState() == Qt::Checked) {
        toInstallList << item->toolTip();
    } else {
        toInstallList.removeAt(toInstallList.indexOf(item->toolTip()));
    }
}

void InstallGui::runPackageInstall()
{
    if (KToolInvocation::kdeinitExec("/usr/lib/kde4/libexec/kdesu", QStringList() << "install-package --install" << toInstallList) == 0) {
        KNotification::event("Install",
                             i18n("You will need to restart %1 to use the new functionality", applicationName),
                             KIcon("system-software-update").pixmap(QSize(22,22)));
    }
}

void InstallGui::cleanUpDialog()
{
    dialog->deleteLater();
    dialog = 0;
}

#include "installgui.moc"
