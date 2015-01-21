/* 	CuteFarm
    Copyright (C) 2008-2011  Jan Vaillant

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation License 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include <QMessageBox>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QDesktopServices>
#include <QDebug>

#include "setupwizard.h"

SetupWizard::SetupWizard(QWidget *parent)
    : QWizard(parent, (Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint))
{
    QWizard::setMinimumWidth(550);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif

    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pic/ladybug50.png"));
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::HaveHelpButton, true);
    setOption(QWizard::IgnoreSubTitles, true);

    setSizeGripEnabled(true);

    setPage(Page_Start, new SetupWizardStartPage(this));
    setPage(Page_Import, new SetupWizardImportPage(this));

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}

void SetupWizard::onCurrentIdChanged(int id)
{
    if (id == Page_Import)
        qobject_cast<SetupWizardImportPage*>(page(Page_Import))->import();

    if (id != Page_Start)
        button(QWizard::CancelButton)->setVisible(false);
}

void SetupWizard::showHelp()
{
    QString message;
    message = tr("If you experience any problems importing your old "
            "CuteFarm database from an earlier version of CuteFarm you should "
            "not hesitate to let me know. I'll try to fix it.");

    QMessageBox::information(this, "CuteFarm", message);
}


SetupWizardStartPage::SetupWizardStartPage(QWidget *parent)
    : QWizardPage(parent)
{
    ui.setupUi(this);
    registerField("file*", ui.lineEdit);

}

void SetupWizardStartPage::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
            QObject::tr("Open Database"),
            QDesktopServices::storageLocation(QDesktopServices::DataLocation),
            "SQLite3 (*.sqlite3)");

    if (path.isEmpty())
        return;

    QSet<int> userVersions;
    userVersions << 190 << 191 << 200;

    int userVersion = 0;
    QSqlQuery q;
    q.exec(QString("ATTACH '%1' AS import").arg(path));
    q.exec("PRAGMA import.user_version");
    while (q.next())
        userVersion = q.value(0).toInt();

    q.exec("DETACH import");

    if (!userVersions.contains(userVersion)) {
        QMessageBox::information(this, "CuteFarm", tr("Sorry, can't import this database.\n"
                "Reason: '%1' is not a valid CuteFarm database version.").arg(userVersion));
        return;
    }

//    qDebug() << "import userVersion" << userVersion;

    ui.lineEdit->setText(path);

}

bool SetupWizardStartPage::isComplete() const
{
    return QWizardPage::isComplete();
}

int SetupWizardStartPage::nextId() const
{
    return SetupWizard::Page_Import;
}


SetupWizardImportPage::SetupWizardImportPage(QWidget *parent)
    : QWizardPage(parent)
{
	ui.setupUi(this);
}

int SetupWizardImportPage::nextId() const
{
    return -1;
}

bool SetupWizardImportPage::isComplete() const
{
    return true;
}

void SetupWizardImportPage::import()
{
    wizard()->button(QWizard::BackButton)->setDisabled(true);
    wizard()->button(QWizard::CommitButton)->setDisabled(true);
    wizard()->button(QWizard::CancelButton)->setDisabled(true);

    QStringList impTables;
    QStringList myTables;

    QSqlQuery q;
    q.exec(QString("ATTACH '%1' AS import").arg(field("file").toString()));
    q.exec("DROP TRIGGER main.seed_on_cultivar_ins");
//    qDebug() <<  q.lastError().databaseText();

    QSqlDatabase::database().transaction();


    q.exec("SELECT tbl_name FROM main.sqlite_master WHERE type='table' AND tbl_name NOT LIKE '%sqlite%'");
    while (q.next())
        myTables << q.value(0).toString();
    q.exec("SELECT tbl_name FROM import.sqlite_master WHERE type='table' AND tbl_name NOT LIKE '%sqlite%'");
    while (q.next()) {
        if (myTables.contains(q.value(0).toString()))
            impTables << q.value(0).toString();
    }

    ui.progressBar->setMaximum(impTables.count());
    ui.progressBar->setValue(0);

    foreach (const QString &table, impTables) {
        QString cols;
        q.exec(QString("PRAGMA import.table_info('%1')").arg(table));
        while(q.next())
            cols.append(q.value(1).toString()).append(',');

        cols.chop(1);
//        qDebug() << table;
        QString del = QString("DELETE FROM main.%1").arg(table);
        q.exec(del);
        if (q.lastError().type() != QSqlError::NoError)
            emit error(table, q.lastError().databaseText());

        QString ins = QString("INSERT INTO main.%1 (%2) SELECT %2 FROM import.%1")
                .arg(table)
                .arg(cols);
        q.exec(ins);
        if (q.lastError().type() != QSqlError::NoError)
            emit error(table, q.lastError().databaseText());

        ui.progressBar->setValue(ui.progressBar->value()+1);
    }

    if (!QSqlDatabase::database().commit()) {
        QString err = QSqlDatabase::database().lastError().databaseText();
        QSqlDatabase::database().rollback();
        q.exec("DETACH import");
        q.exec("CREATE TRIGGER seed_on_cultivar_ins "
          " AFTER INSERT ON cultivar "
          " BEGIN "
          " INSERT INTO seed (cultivar_id) VALUES(new.cultivar_id); "
          " END; ");

        QMessageBox::information(this, "CuteFarm", tr("Commit error:\n%1").arg(err));
        setTitle(tr("Import failed."));
        wizard()->button(QWizard::BackButton)->setVisible(true);
    }
    else {
        setTitle(tr("Import finished."));
        q.exec("DETACH import");
        q.exec("CREATE TRIGGER seed_on_cultivar_ins "
          " AFTER INSERT ON cultivar "
          " BEGIN "
          " INSERT INTO seed (cultivar_id) VALUES(new.cultivar_id); "
          " END; ");

        wizard()->button(QWizard::BackButton)->setVisible(false);
    }

    q.exec("DETACH import");

    wizard()->button(QWizard::BackButton)->setEnabled(true);
    wizard()->button(QWizard::CommitButton)->setEnabled(true);
    wizard()->button(QWizard::CancelButton)->setEnabled(true);
}


