/***************************************************************************
 *   Copyright © 2009-2010 Jonathan Thomas <echidnaman@kubuntu.org>        *
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
#include <QDialog>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>

// KDE includes
#include <KLocalizedString>
#include <KNotification>

InstallGui::InstallGui(QObject* parent, const QString &application, const QMap<QString, QString> packageList)
        : QObject(parent)
        , m_dialog(0)
        , m_installProcess(0)
        , m_applicationName(application)
{
    m_dialog = new QDialog();
    m_dialog->setWindowIcon(QIcon::fromTheme("muondiscover"));
    m_dialog->setWindowTitle(i18n("Install Packages"));
    QVBoxLayout *layout = new QVBoxLayout(m_dialog);
    m_dialog->setLayout(layout);

    QLabel *label = new QLabel(m_dialog);
    label->setWordWrap(true);
    label->setText(i18n("Select packages to be installed for extra functionality."
                        " These packages are not installed by default due to either patent"
                        " issues, restrictive licensing or a lack of space on the installation"
                        " media."));
    layout->addWidget(label);

    QListWidget *listWidget = new QListWidget(m_dialog);
    connect(listWidget, SIGNAL(itemChanged(QListWidgetItem *)), SLOT(packageToggled(QListWidgetItem *)));
    layout->addWidget(listWidget);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_dialog);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(runPackageInstall()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(cleanUpDialog()));
    layout->addWidget(m_buttonBox);
    
    QMap<QString, QString>::const_iterator nameIter = packageList.constBegin();
    while (nameIter != packageList.constEnd()) {
        QListWidgetItem *item = new QListWidgetItem(nameIter.value());
        item->setToolTip(nameIter.key());
        m_toInstallList << nameIter.key();
        item->setCheckState(Qt::Checked);
        listWidget->addItem(item);
        ++nameIter;
    }

    m_dialog->show();
}

InstallGui::~InstallGui()
{
    delete m_dialog;
}

void InstallGui::packageToggled(QListWidgetItem *item)
{
    QString packageName = item->toolTip();
    if (item->checkState() == Qt::Checked) {
        m_toInstallList << packageName;
    } else {
        m_toInstallList.removeOne(packageName);
    }
#warning fixme
//     m_dialog->button(QDialog::Ok)->setDisabled(m_toInstallList.isEmpty());
    m_buttonBox->button(QDialogButtonBox::Ok)->setDisabled(m_toInstallList.isEmpty());
}

void InstallGui::runPackageInstall()
{
    m_dialog->accept();
    m_installProcess = new QProcess(this);
    connect(m_installProcess, SIGNAL(finished(int)), this, SLOT(installFinished(int result)));

    m_installProcess->start("qapt-batch", QStringList() << "--install" << m_toInstallList);
}

void InstallGui::installFinished(int result)
{
    if (result == 1) {
        // QApt Batch will handle error notification, we just need to know to shut up
        return;
    }

    KNotification *notify = new KNotification("Install", 0);
    notify->setComponentName("notificationhelper");

    notify->setPixmap(QIcon::fromTheme("download").pixmap(22,22));
    notify->setText(i18n("Installation complete. You will need to restart %1"
                         " to use the new functionality", m_applicationName));
    notify->sendEvent();
}

void InstallGui::cleanUpDialog()
{
    m_dialog->deleteLater();
    m_dialog = 0;
}

#include "installgui.moc"
