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

#ifndef FIELDVIEWWIDGET_H
#define FIELDVIEWWIDGET_H

#include <QtGui/QWidget>
#include <QSqlQueryModel>

#include "sqlquerymodel.h"
#include "ui_fieldviewwidget.h"

class FieldViewWidget : public QWidget
{
    Q_OBJECT

public:
    FieldViewWidget(QWidget *parent = 0);
    ~FieldViewWidget();

    Ui::FieldViewWidgetClass ui;

    void refreshTables();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

signals:
    void actionTblContextMenuRequested(const QPoint &pos);

public slots:
    void on_cBFertBalanceYears_toggled(bool on);
    void onComboBoxFildCurrentIndexChanged(int idx);
    void on_comboBoxYear_currentIndexChanged(const QString &text);
    void refresh();
    void onDatabaseChanged();
    void onSettingsChanged();

private:
    void setupModels();
    void setModelHeaders();
    void updateFertBalance();
    void resetFertBalance();

    QSqlQueryModel *mFildModel;
    QSqlQueryModel *yearModel;
    QSqlQueryModel *fildInfo;
    SqlQueryModel *drilActnModel;
    SqlQueryModel *harvActnModel;
    SqlQueryModel *soilActnModel;
    SqlQueryModel *medwActnModel;
    SqlQueryModel *pltrActnModel;
    SqlQueryModel *fertActnModel;
    SqlQueryModel *animActnModel;
    SqlQueryModel *miscActnModel;
    SqlQueryModel *soilSampleActnModel;



};

#endif // FIELDVIEWWIDGET_H
