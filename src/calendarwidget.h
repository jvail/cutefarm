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

#ifndef CALENDARWIDGET_H_
#define CALENDARWIDGET_H_

#include <QtGui>

#include "cutefarm.h"

struct OprtDetail {
    int actnId;
    QString oprtType;
    QString workName;
    QString oprtName;
};

class CalendarWidget : public QCalendarWidget
{
	Q_OBJECT

public:
	CalendarWidget(QWidget *parent = 0);
	~CalendarWidget();

	void setSelectedActnId(int actnId);
	void updateCalendar() { on_currentPageChanged(); }
	int currentActnId();
	QString currentActnText();

protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
    bool eventFilter(QObject *obj, QEvent *event);
    void showEvent ( QShowEvent *event);

signals:
	void posClicked(int actn_id);
	void statusMsg(const QString &msg, int timeout);
    void actionTblContextMenuRequested(const QPoint &pos);

private slots:
	void on_currentPageChanged();
	void loadData();
	void paintAfterResize();

private:
	QString oprtPixmap(const QString &oprt, bool large = false) const;
	QString oprtData(const QString &oprt, int actnId);

    QMultiMap<QDate, OprtDetail> mActnDates; //enthält actn_date + ('is_xxxx', actn_id, ... etc.)
    mutable QMultiHash<int, QRect > mRectsHash; //enthält actn_id + rect
    mutable QHash<int, QString > mDataHash; //enthält actn_id + daten zur actn
    mutable QMultiHash<QString, QRect > mRectDataHash; //'is_xxxx' + rect (um größeres Bild zu laden)

    QRect mCurrentRect;
    QString mCurrentToolTipText;
    int mCurrentYear;
    int mCurrentMonth;

    QAbstractItemView *mCalendarTableView;
    QFont mCalendarCellFont;

    QTimer *mLoadDataTimer;
    QTimer *mPaintAfterResizeTimer;

    QSize mNewSize;
    int mAvailableRows;
    int mAvailableCols;

    int mSelectedActnId;
    QDate mSelectedDate;
    mutable QDate mPreviousSelectedDate;

    bool mResizeAllowed;
    bool mIsMouseRelease;
};

#endif /*CALENDARWIDGET_H_*/
