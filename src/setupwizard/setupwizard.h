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

#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <QWizard>

#include "../ui_setupwizardstartpage.h"
#include "../ui_setupwizardimportpage.h"


class SetupWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Start, Page_Import };

    SetupWizard(QWidget *parent = 0);

private slots:
    void onCurrentIdChanged(int id);
    void showHelp();

};


class SetupWizardStartPage : public QWizardPage
{
    Q_OBJECT

public:

    SetupWizardStartPage(QWidget *parent = 0);

    int nextId() const;

private slots:
    void on_pushButton_clicked();

private:
    bool isComplete() const;

    Ui::SetupWizardStartPageClass ui;
};


class SetupWizardImportPage : public QWizardPage
{
    Q_OBJECT

public:

    SetupWizardImportPage(QWidget *parent = 0);

    int nextId() const;
    void import();

signals:
    void error(const QString &table, const QString &err);

private:
	bool isComplete() const;

    Ui::SetupWizardImportPageClass ui;
};


#endif
