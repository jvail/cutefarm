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

#ifndef CALENDARVIEWWIDGET_H
#define CALENDARVIEWWIDGET_H

#include <QtGui/QWidget>

#include "sqlquerymodel.h"
#include "calendarwidget.h"
#include "ui_calendarviewwidget.h"

class CalendarViewWidget : public QWidget
{
    Q_OBJECT

public:
    CalendarViewWidget(QWidget *parent = 0);
    ~CalendarViewWidget();

    Ui::CalendarViewWidgetClass ui;

    QDate selectedDate();

signals:
    void statusMsg(const QString &msg, int timeout);



private:
//	SqlQueryModel *actnDetailModel;
//	CalendarDetailDialog *calDetail;


};

#endif // CALENDARVIEWWIDGET_H
