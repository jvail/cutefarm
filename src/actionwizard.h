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

#ifndef ACTIONWIZARD_H
#define ACTIONWIZARD_H

#include <QWizard>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include "settingsdialog.h"
#include "spinboxdelegate.h"
#include "ui_actionwizardactnpage.h"
#include "ui_actionwizardfildpage.h"
#include "ui_actionwizardcomtpage.h"
#include "ui_actionwizardanimpage.h"
#include "ui_actionwizarddrilpage.h"
#include "ui_actionwizardharvpage.h"
#include "ui_actionwizardfertpage.h"
#include "ui_actionwizardpltrpage.h"
#include "ui_actionwizardsoiltestpage.h"

class ActionWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Action, Page_Fild, Page_Comment, Page_Animal,
    	   Page_Drill, Page_Harvest, Page_Fertiliser,
           Page_Planttreatment, Page_SoilTesting };

    enum Mode {New, Update, Copy };

    enum OprtList { Oprt_Soil, Oprt_Medw, Oprt_Pltr, Oprt_Fert,
    	            Oprt_Dril, Oprt_Harv, Oprt_Anim, Oprt_Misc, Oprt_SoilTesting };

    enum ListType { List_Fild, List_Oprt, List_PagesToVisit };

    ActionWizard(QWidget *parent = 0, int actnId = -1, Mode mode = ActionWizard::New,
            const QDate &date = QDate::currentDate());

    QList<int> list(int ListType) const;
    QList<QString> acntList() const;
    QMultiMap<QString, QString> acntMap() const;

    ActionWizard::Mode currentMode() const;

signals:
//	void signalAcntList(const QList<QString> &list);
	void signalAcntMap(const QMultiMap<QString, QString> &map);


private slots:
	void finish();
    void showHelp();
    void onCurrentIdChanged(int id);

private:
	void loadActnData();
	int m_actnId;
	Mode m_mode;
	int m_year;
	QStandardItemModel *fildModel;
	QStandardItemModel *filteredFildModel;
	QSortFilterProxyModel *proxyModel;
	QStandardItemModel *pltrModel;
	QStandardItemModel *fertModel;
	QStandardItemModel *drilModel;
	QStandardItemModel *harvModel;
	QStandardItemModel *animModel;
	QList<QString> m_actnList;
	QMultiMap<QString, QString> m_actnMap;

};


class ActionWizardActnPage : public QWizardPage
{
    Q_OBJECT

public:
//    enum ListType { List_Fild, List_Oprt, List_PagesToVisit };

	ActionWizardActnPage(const QDate &date, QWidget *parent = 0);

    int nextId() const;
    QList<int> dataList(int ListType) const;
    int workerId();
    void setWorkerId(int id);
    QString workName();
    QString date();
    void setDate(QString date);
    int oprtId();
    void setOprtId(int id);
    QString oprtName();
    void setMiscChecked(bool checked);
    void setPltrChecked(bool checked);
    void setFertChecked(bool checked);
    void setDrilChecked(bool checked);
    void setHarvChecked(bool checked);
    void setSoilChecked(bool checked);
    void setMedwChecked(bool checked);
    void setAnimChecked(bool checked);
    void setSoilTestChecked(bool checked);

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;

//	QStandardItemModel *prodFildModel;
	QSqlQueryModel *oprtModel;
	QSqlQueryModel *workModel;

    Ui::ActionWizardActnPageClass ui;
};


class ActionWizardFildPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardFildPage(QWidget *parent = 0, QStandardItemModel *fildModel = 0, int year = -1);

    int nextId() const;
    int year();

protected:
	void initializePage();

private slots:
	void on_comboBoxYear_currentIndexChanged(const QString &year);
	void emitCompleteChanged();
	void selectAll(int state);

private:
	bool isComplete() const;
	QLocale locale;
	QStandardItemModel *m_fildModel;
	QSqlQueryModel *yearModel;

	Ui::ActionWizardFildPageClass ui;
};


class ActionWizardAnimPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardAnimPage(QWidget *parent = 0, QStandardItemModel *animModel = 0);

    int nextId() const;

protected:
	void initializePage();

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;
	QStandardItemModel *m_animModel;

	Ui::ActionWizardAnimPageClass ui;
};


class ActionWizardDrilPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardDrilPage(QWidget *parent = 0, QStandardItemModel *drilModel = 0);

    int nextId() const;

protected:
	void initializePage();

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;
	QStandardItemModel *m_drilModel;

	Ui::ActionWizardDrilPageClass ui;
};


class ActionWizardHarvPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardHarvPage(QWidget *parent = 0, QStandardItemModel *harvModel = 0);

    int nextId() const;

protected:
	void initializePage();

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;
	QStandardItemModel *m_harvModel;
	Ui::ActionWizardHarvPageClass ui;
};


class ActionWizardFertPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardFertPage(QWidget *parent = 0, QStandardItemModel *fertModel = 0);

    int nextId() const;

protected:
	void initializePage();

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;
	QStandardItemModel *m_fertModel;

	Ui::ActionWizardFertPageClass ui;
};


class ActionWizardPltrPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardPltrPage(QWidget *parent = 0,
			QStandardItemModel *pltrModel = 0);

    int nextId() const;

protected:
	void initializePage();

private slots:
	void emitCompleteChanged();

private:
	bool isComplete() const;
	QStandardItemModel *m_pltrModel;

	Ui::ActionWizardPltrPageClass ui;
};

class ActionWizardSoilTestPage : public QWizardPage
{
    Q_OBJECT

public:
    ActionWizardSoilTestPage(QWidget *parent = 0);

    Ui::ActionWizardSoilTestPageClass ui;

    int nextId() const;

protected:
    void initializePage();

private:
    bool isComplete() const;

};

class ActionWizardComtPage : public QWizardPage
{
    Q_OBJECT

public:
	ActionWizardComtPage(QWidget *parent, QSortFilterProxyModel *proxy);

    int nextId() const;

    void setVisible(bool visible);
    QString actnComment();
    void setActnComt(QString comt);

protected:
	void initializePage();

private slots:
	void toggleFildCom(int state);

private:
	QSortFilterProxyModel *mProxy;

	Ui::ActionWizardComtPageClass ui;
};


#endif
