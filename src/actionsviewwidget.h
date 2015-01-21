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

#ifndef ACTIONSVIEWWIDGET_H
#define ACTIONSVIEWWIDGET_H

#include <QDateEdit>

#include "ui_actionsviewwidget.h"

class QToolBar;

class ActionsViewWidget : public QWidget
{
    Q_OBJECT

public:
    ActionsViewWidget(QWidget *parent = 0);
    ~ActionsViewWidget();

    Ui::ActionsViewWidgetClass ui;

    QString htmlTable() const;
    QDate startDate() const { return mDateEditStart->date(); };
    QDate endDate() const { return mDateEditEnd->date(); };


public slots:
    void refresh();
    void onDatabaseChanged();
    void onActionFilterToggled(bool on);
    void onExpandAllTriggered();
    void onCollapseAllTriggered();

private:
    QToolBar *mFilterBar;
    QDateEdit *mDateEditStart;
    QDateEdit *mDateEditEnd;

};

#endif // ACTIONSVIEWWIDGET_H
