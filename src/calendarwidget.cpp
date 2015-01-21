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

#include "calendarwidget.h"

#include <QSqlQuery>
#include <QSqlError>


CalendarWidget::CalendarWidget(QWidget *parent)
	: QCalendarWidget(parent), mSelectedActnId(-1)
{
	mIsMouseRelease = false;
	mPreviousSelectedDate = selectedDate();

	mCalendarTableView = qFindChild<QTableView *>(this);
	mCalendarTableView->viewport()->installEventFilter(this);
	mCalendarTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	this->setFirstDayOfWeek(Qt::Monday);
	this->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	this->setHorizontalHeaderFormat(QCalendarWidget::NoHorizontalHeader);
	this->setGridVisible(false);

	setMouseTracking(true);

	mCalendarCellFont.setPointSize(13);

	connect(this, SIGNAL(currentPageChanged(int,int)), this, SLOT(on_currentPageChanged()));

	mLoadDataTimer = new QTimer(this);
	mLoadDataTimer->setSingleShot(true);
	connect(mLoadDataTimer, SIGNAL(timeout()), this, SLOT(loadData()));

	mPaintAfterResizeTimer = new QTimer(this);
	mPaintAfterResizeTimer->isSingleShot();
	mPaintAfterResizeTimer->setInterval(500);
	connect(mPaintAfterResizeTimer, SIGNAL(timeout()), this, SLOT(paintAfterResize()));

	mResizeAllowed = false;
}

void CalendarWidget::paintAfterResize()
{
//	qDebug() << "now repaint";
	mPaintAfterResizeTimer->stop();
	mResizeAllowed = true;

	//calculate columns
	int width = mNewSize.width();
	mAvailableCols = ((width/7)-5)/18;
//	qDebug() << "availableCols " << availableCols;
	//calculate rows
	int height = mNewSize.height();
	mAvailableRows = (((height-16)/6)-24)/18;
//	qDebug() << "availableRows " << availableRows;

	mRectsHash.clear();
	mRectDataHash.clear();
	mDataHash.clear();

	this->resize(mNewSize.width(), mNewSize.height()+16);
	mResizeAllowed = false;
}

CalendarWidget::~CalendarWidget()
{

}

void CalendarWidget::setSelectedActnId(int actnId)
{
//	if (actnId < 0)
//		return;
//
////	qDebug() << "m_selectedActnId" << actnId;
//	mSelectedActnId = actnId;
}

int CalendarWidget::currentActnId()
{
    qDebug() << "currentActnId" << mSelectedActnId;
    return mSelectedActnId;
}

QString CalendarWidget::currentActnText()
{
    if (mSelectedActnId < 0)
        return QString();
    else
        return mDataHash.value(mSelectedActnId);
}

QString CalendarWidget::oprtPixmap(const QString &oprt, bool large) const
{
    QString pix;

    if (oprt == "fert")
        pix = (large) ? ":/pic/fert.png" : ":/pic/fert16.png";
    else if (oprt == "pltr")
        pix = (large) ? ":/pic/pltr32.png" : ":/pic/pltr32.png";
    else if (oprt == "anim")
        pix = (large) ? ":/pic/cow32.png" : ":/pic/cow16.png";
    else if (oprt == "medw")
        pix = (large) ? ":/pic/meadow.png" : ":/pic/meadow16.png";
    else if (oprt == "misc")
        pix = (large) ? ":/pic/comment.png" : ":/pic/comment16.png";
    else if (oprt == "soil")
        pix = (large) ? ":/pic/soilfield.png" : ":/pic/soilfield16.png";
    else if (oprt == "dril")
        pix = (large) ? ":/pic/crop32.png" : ":/pic/crop16.png";
    else if (oprt == "harv")
        pix = (large) ? ":/pic/cut.png" : ":/pic/cut16.png";
    else if (oprt == "soil_test")
        pix = (large) ? ":/pic/soiltesting32.png" : ":/pic/soiltesting16.png";
    else
        pix = ":/pic/comment16.png";

    return pix;
}

void CalendarWidget::showEvent(QShowEvent *event)
{
	on_currentPageChanged();
	QCalendarWidget::showEvent(event);
}

bool CalendarWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Resize && mResizeAllowed == false) {
		mPaintAfterResizeTimer->start();
		QResizeEvent *re = static_cast<QResizeEvent*>(event);
		mNewSize = QSize(re->size().width(), re->size().height()+ (height()-mCalendarTableView->viewport()->height())-18);
		return true;
	}

	if (!obj->parent() && obj->parent() != mCalendarTableView)
		return QObject::eventFilter(obj, event);

//	qDebug() << event << obj;

	if (event->type() == QEvent::ContextMenu) {
	    qDebug() << "context";
		QContextMenuEvent *ce = static_cast<QContextMenuEvent*>(event);
		mSelectedActnId = -1;
        QHashIterator<int, QRect > i(mRectsHash);
        while (i.hasNext()) {
            i.next();
            if (i.value().contains(ce->pos())) {
                mSelectedActnId = i.key();
                break;
            }
        }

        emit actionTblContextMenuRequested(ce->globalPos());
        return QObject::eventFilter(obj, event);
	}

    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *he = static_cast<QHelpEvent*>(event);

//        qDebug() << "QEvent::ToolTip";

        QPoint mousePos = he->pos();

        if (!mCurrentRect.isEmpty() && mCurrentRect.contains(mousePos)) {
            QToolTip::showText(he->globalPos(), mCurrentToolTipText);
            return true;
        }

        QHashIterator<int, QRect > i(mRectsHash);
        while (i.hasNext()) {
            i.next();
            if (i.value().contains(mousePos)) {
                mCurrentRect = i.value();
                QString oprt;
                QString pic;
                QString header = "<p style=\"font-size: large; font-weight: bold; margin-top:5px;margin-left:5px;\"><nobr>" +
                        mDataHash.value(i.key()) + "</nobr></p>";

                QHashIterator<QString, QRect > i2(mRectDataHash);
                while (i2.hasNext()) {
                    i2.next();
                    if (mCurrentRect == i2.value()) {
                        pic = QString("<img src=\"%1\"/>").arg(oprtPixmap(i2.key(), true));
                        oprt = oprtData(i2.key(), i.key());
                        break;
                    }
                }

                mCurrentToolTipText = "<table><tr><td valign=\"middle\">" + pic + "</td><td>" + header + oprt + "</td></tr></table>";
                QToolTip::showText(he->globalPos(), mCurrentToolTipText);
//                qDebug() << mCurrentToolTipText;
                return true;
            }
        }

        QToolTip::hideText();

    }

	return QObject::eventFilter(obj, event);
}

void CalendarWidget::on_currentPageChanged()
{
	if (!isVisible())
		return;

	mCurrentRect = QRect();
	mRectsHash.clear();
	mActnDates.clear();
	mRectDataHash.clear();
	mDataHash.clear();
    mLoadDataTimer->start(1000);
}

QString CalendarWidget::oprtData(const QString &oprt, int actnId)
{
    if (oprt.isEmpty())
        return "Error: No Operation found";


//    qDebug() << "oprtData" << oprt << actnId;

	emit statusMsg(tr("Loading..."), 1000);
	QSqlQuery q;
	QString data;

	if (!QSqlDatabase::database().transaction()) {
		QSqlDatabase::database().rollback();
		QSqlDatabase::database().transaction();
	}

	if (oprt != "medw" && oprt != "soil" && oprt != "misc" && oprt != "soil_test" ) {
		if (oprt == "dril") {
			q.prepare( " SELECT group_concat(seed_name, ', ') "
					   " FROM v_seed, actn_dril "
					   " WHERE actn_id=:actn_id "
					   " AND actn_dril.seed_id= v_seed.seed_id "
					   " GROUP BY actn_id");
			q.bindValue(":actn_id", actnId);
			q.exec();
		}
		else if (oprt == "anim") {
			q.prepare( " SELECT group_concat(anim_grp_name, ', ') "
					   " FROM anim_grp, actn_anim_grp "
					   " WHERE actn_id=:actn_id "
					   " AND actn_anim_grp.anim_grp_id=anim_grp.anim_grp_id "
					   " GROUP BY actn_id");
			q.bindValue(":actn_id", actnId);
			q.exec();
		}
		else if (oprt == "harv") {
			q.prepare( " SELECT group_concat(crop_use_name, ', ') "
					   " FROM crop_use, actn_harv "
					   " WHERE actn_id=:actn_id "
					   " AND actn_harv.crop_use_id=crop_use.crop_use_id "
					   " GROUP BY actn_id");
			q.bindValue(":actn_id", actnId);
			q.exec();
		}
		else {
			q.prepare( " SELECT group_concat(" + oprt+"_name, ', ' )"
					   " FROM "+oprt +", "+"actn_"+oprt +
					   " WHERE actn_id=:actn_id "
					   " AND " + "actn_"+oprt+"."+oprt+"_id" +" = "+oprt+"."+oprt+"_id "
					   " GROUP BY actn_id");
			q.bindValue(":actn_id", actnId);
			q.exec();
		}

        if (q.lastError().type() != QSqlError::NoError)
              qDebug() << q.lastError().text() << q.lastQuery();

		while (q.next())
			data = "<p style=\"margin-left:5px;\">" + q.value(0).toString() +"</p>";
	}

	q.prepare(" SELECT group_concat(DISTINCT  ' ' || fild_name) " //, area , input_name, input_amt, sum_amt, fild_com
			" FROM v_actn_detail "
			" WHERE actn_id=:actn_id "
			" GROUP BY actn_id");
	q.bindValue(":actn_id", actnId);
	q.exec();

	int count = 1;
	while (q.next()) {
	    data.append("<p style=\"margin-left:5px;\">" + q.value(0).toString().remove(0, 1) +"</p>");
		count++;
	}

    if (q.lastError().type() != QSqlError::NoError)
        qDebug() << q.lastError().text();

	QSqlDatabase::database().commit();

	if (data.isEmpty())
		data = tr("No data available.");

	emit statusMsg("Ok", 1000);

	return data;
}

void CalendarWidget::loadData()
{
	emit statusMsg(tr("Loading..."), 0);
	QString month;
	QDate date(yearShown(), monthShown(), 1);
	QDate startDate = date.addDays(-11);
	QDate endDate = date.addDays(42);
	QSqlQuery q;

	bool b  = QSqlDatabase::database().transaction();

	if (!b) {
		QSqlDatabase::database().rollback();
		QSqlDatabase::database().transaction();
	}

	q.prepare( " SELECT actn_id, actn_date, 'fert', work_name, oprt_name "
			   " FROM actn, work, oprt "
			   " WHERE actn_id IN ( SELECT actn_id "
	 	                          " FROM actn_fert "
                                  " WHERE actn_id IN ( SELECT actn_id "
		                                             " FROM actn "
                                                     " WHERE date(actn_date) > :startDate1 "
	                                                 " AND date(actn_date) < :endDate1 ) ) "
               " AND actn.work_id=work.work_id "
               " AND actn.oprt_id=oprt.oprt_id "
               " UNION ALL"
               " SELECT actn_id, actn_date, 'pltr', work_name, oprt_name "
               " FROM actn, work, oprt "
               " WHERE actn_id IN ( SELECT actn_id "
	 	                          " FROM actn_pltr "
                                  " WHERE actn_id IN ( SELECT actn_id "
		                                             " FROM actn "
                                                     " WHERE date(actn_date) > :startDate2 "
	                                                 " AND date(actn_date) < :endDate2 ) ) "
               " AND actn.work_id=work.work_id "
               " AND actn.oprt_id=oprt.oprt_id "
			   " UNION ALL"
               " SELECT actn_id, actn_date, 'dril', work_name, oprt_name "
               " FROM actn, work, oprt "
               " WHERE actn_id IN ( SELECT actn_id "
	 	                          " FROM actn_dril "
                                  " WHERE actn_id IN ( SELECT actn_id "
		                                             " FROM actn "
                                                     " WHERE date(actn_date) > :startDate3 "
	                                                 " AND date(actn_date) < :endDate3 ) ) "
              " AND actn.work_id=work.work_id "
              " AND actn.oprt_id=oprt.oprt_id "
			  " UNION ALL"
              " SELECT actn_id, actn_date, 'harv', work_name, oprt_name "
              " FROM actn, work, oprt "
              " WHERE actn_id IN ( SELECT actn_id "
	 	                         " FROM actn_harv "
                                 " WHERE actn_id IN ( SELECT actn_id "
		                                             " FROM actn "
                                                     " WHERE date(actn_date) > :startDate4 "
	                                                 " AND date(actn_date) < :endDate4 ) ) "
             " AND actn.work_id=work.work_id "
             " AND actn.oprt_id=oprt.oprt_id "
			 " UNION ALL"
             " SELECT actn_id, actn_date, 'anim', work_name, oprt_name "
             " FROM actn, work, oprt "
             " WHERE actn_id IN ( SELECT actn_id "
	 	                        " FROM actn_anim_grp "
                                " WHERE actn_id IN ( SELECT actn_id "
		                                           " FROM actn "
                                                   " WHERE date(actn_date) > :startDate5 "
	                                               " AND date(actn_date) < :endDate5 ) ) "
             " AND actn.work_id=work.work_id "
             " AND actn.oprt_id=oprt.oprt_id "
			 " UNION ALL"
             " SELECT actn_id, actn_date, 'soil', work_name, oprt_name "
             " FROM actn, work, oprt "
             " WHERE actn_id IN ( SELECT actn_id "
	 	                        " FROM actn_soil "
                                " WHERE actn_id IN ( SELECT actn_id "
		                                           " FROM actn "
                                                   " WHERE date(actn_date) > :startDate6 "
	                                               " AND date(actn_date) < :endDate6 ) ) "
             " AND actn.work_id=work.work_id "
             " AND actn.oprt_id=oprt.oprt_id "
			 " UNION ALL"
             " SELECT actn_id, actn_date, 'medw', work_name, oprt_name "
             " FROM actn, work, oprt "
             " WHERE actn_id IN ( SELECT actn_id "
	 	                        " FROM actn_medw "
                                " WHERE actn_id IN ( SELECT actn_id "
		                                           " FROM actn "
                                                   " WHERE date(actn_date) > :startDate7 "
	                                               " AND date(actn_date) < :endDate7 ) ) "
             " AND actn.work_id=work.work_id "
             " AND actn.oprt_id=oprt.oprt_id "
			 " UNION ALL"
             " SELECT actn_id, actn_date, 'misc', work_name, oprt_name "
             " FROM actn, work, oprt "
             " WHERE actn_id IN ( SELECT actn_id "
	 	                        " FROM actn_misc "
                                " WHERE actn_id IN ( SELECT actn_id "
		                                           " FROM actn "
                                                   " WHERE date(actn_date) > :startDate8 "
	                                               " AND date(actn_date) < :endDate8 ) ) "
             " AND actn.work_id=work.work_id "
             " AND actn.oprt_id=oprt.oprt_id "
             " UNION ALL"
             " SELECT actn_id, actn_date, 'soil_test', work_name, oprt_name "
             " FROM actn, work, oprt "
             " WHERE actn_id IN ( SELECT actn_id "
                               " FROM actn_soil_test "
                               " WHERE actn_id IN ( SELECT actn_id "
                                                  " FROM actn "
                                                  " WHERE date(actn_date) > :startDate9 "
                                                  " AND date(actn_date) < :endDate9 ) ) "
            " AND actn.work_id=work.work_id "
            " AND actn.oprt_id=oprt.oprt_id "
	        );
	QString dateStart = startDate.toString("yyyy-MM-dd");
	QString dateEnd = endDate.toString("yyyy-MM-dd");
	q.bindValue(":startDate1", dateStart);
	q.bindValue(":endDate1", dateEnd);
	q.bindValue(":startDate2", dateStart);
	q.bindValue(":endDate2", dateEnd);
	q.bindValue(":startDate3", dateStart);
	q.bindValue(":endDate3", dateEnd);
	q.bindValue(":startDate4", dateStart);
	q.bindValue(":endDate4", dateEnd);
	q.bindValue(":startDate5", dateStart);
	q.bindValue(":endDate5", dateEnd);
	q.bindValue(":startDate6", dateStart);
	q.bindValue(":endDate6", dateEnd);
	q.bindValue(":startDate7", dateStart);
	q.bindValue(":endDate7", dateEnd);
	q.bindValue(":startDate8", dateStart);
	q.bindValue(":endDate8", dateEnd);
    q.bindValue(":startDate9", dateStart);
    q.bindValue(":endDate9", dateEnd);
	q.exec();


	while (q.next()) {
		OprtDetail detail;
		detail.actnId = q.value(0).toInt();
        detail.oprtType = q.value(2).toString();
        detail.workName = q.value(3).toString();
		detail.oprtName = q.value(4).toString();
		mActnDates.insert(QDate::fromString(q.value(1).toString(), "yyyy-MM-dd"), detail);
	}

	if (q.lastError().type() == QSqlError::NoError)
	    qDebug() << q.lastError().text();

	QSqlDatabase::database().commit();


	if (!mActnDates.isEmpty()) {
	    if (mCalendarTableView) {
	    	mCalendarTableView->viewport()->update();
	    }
	    else
	    	update();
	}

	emit statusMsg(tr("Loaded"), 500);
}

void CalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
	painter->setFont(mCalendarCellFont);

    if (date == selectedDate()) {
    	painter->save();
    	painter->fillRect(rect, QBrush(Qt::white));

    	painter->setPen(QPen(Qt::lightGray));
    	painter->setRenderHint ( QPainter::Antialiasing, true );
    	QRectF rectf = QRectF( rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);
    	QPainterPath path;
    	path.addRoundRect(rectf, rect.height()/12, rect.width()/12);
    	painter->fillPath(path, QBrush(Qt::lightGray));
    	painter->setRenderHint ( QPainter::Antialiasing, false );
    	painter->setPen(Qt::white);
    	painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, QString::number(date.day()));
    	painter->restore();
    }
    else{
    	painter->save();
    	QColor color(240, 240, 240, 255);
    	painter->setRenderHint ( QPainter::Antialiasing, true );
    	QRectF rectf = QRectF( rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);
    	QPainterPath path;
    	path.addRoundRect(rectf, rect.height()/12, rect.width()/12);
    	painter->fillPath(path, QBrush(color));
    	painter->setRenderHint ( QPainter::Antialiasing, false );
    	painter->restore();
    }


    if (mActnDates.contains(date)) {
        QList<OprtDetail> events = mActnDates.values(date);
        QRect br;
        int row1 = 0;
        int row2 = 0;
        int row3 = 0;
        int row4 = 0;
        int col = 18;
        int i = 0;

        foreach (const OprtDetail &event, events) {
            br = QRect(0, 0, 16, 16);
            if (i < mAvailableCols) {
                painter->drawPixmap(QRect(rect.x() + 3 + row1, rect.y()+col, 16, 16), QPixmap(oprtPixmap(event.oprtType)).scaledToHeight(16, Qt::SmoothTransformation), br);
                if (!mIsMouseRelease) {
                mRectsHash.insert(event.actnId, QRect(rect.topLeft().x() + 3 + row1, rect.topLeft().y()+col, 16, 16));
                mDataHash.insert(event.actnId, event.workName + " - " + event.oprtName);
                mRectDataHash.insert(event.oprtType, QRect(rect.topLeft().x() + 3 + row1, rect.topLeft().y()+col, 16, 16));
                }
//                if (event.actnId == mSelectedActnId) {
//                    painter->save();
//                    if (date == selectedDate())
//                        painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DotLine));
//                    else
//                        painter->setPen(QPen(QBrush(Qt::lightGray), 1, Qt::DotLine));
//                QRect br2 = QRect(0, 0, 14, 14);
//                painter->drawPixmap(QRect(rect.x() + 3 + 6 + row1, rect.y()+col + 6, 14, 14), QPixmap(":/pic/services.png").scaledToHeight(14, Qt::SmoothTransformation), br);
//                painter->restore();
//                mPreviousSelectedDate = date;
//                }
                row1 = row1+18;
             }

            else if (i < mAvailableCols*2 && mAvailableRows > 1) {
                painter->drawPixmap(QRect(rect.x() + 3 + row2, rect.y()+(col*2), 16, 16), QPixmap(oprtPixmap(event.oprtType)).scaledToHeight(16, Qt::SmoothTransformation), br);
                if (!mIsMouseRelease) {
                mRectsHash.insert(event.actnId, QRect(rect.topLeft().x() + 3 + row2, rect.topLeft().y()+(col*2), 16, 16));
                mDataHash.insert(event.actnId, event.workName + " - " + event.oprtName);
                mRectDataHash.insert(event.oprtType, QRect(rect.topLeft().x() + 3 + row2, rect.topLeft().y()+(col*2), 16, 16));
                }
//                if (event.actnId == mSelectedActnId) {
//                    painter->save();
//                    if (date == selectedDate())
//                        painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DotLine));
//                    else
//                        painter->setPen(QPen(QBrush(Qt::lightGray), 1, Qt::DotLine));
//                QRect br2 = QRect(0, 0, 14, 14);
//                painter->drawPixmap(QRect(rect.x() + 3 + 6 + row2, rect.y()+(col*2) + 6, 14, 14), QPixmap(":/pic/services.png").scaledToHeight(14, Qt::SmoothTransformation), br);
//                painter->restore();
//                mPreviousSelectedDate = date;
//                }
                row2 = row2+18;
            }

            else if (i < mAvailableCols*3 && mAvailableRows > 2) {

                painter->drawPixmap(QRect(rect.x() + 3 + row3, rect.y()+(col*3), 16, 16), QPixmap(oprtPixmap(event.oprtType)).scaledToHeight(16, Qt::SmoothTransformation), br);
                if (!mIsMouseRelease) {
                    mRectsHash.insert(event.actnId, QRect(rect.topLeft().x() + 3 + row3, rect.topLeft().y()+(col*3), 16, 16));
                    mDataHash.insert(event.actnId, event.workName + " - " + event.oprtName);
                    mRectDataHash.insert(event.oprtType, QRect(rect.topLeft().x() + 3 + row3, rect.topLeft().y()+(col*3), 16, 16));
                 }
//                 if (event.actnId == mSelectedActnId) {
//                    painter->save();
//                    if (date == selectedDate())
//                        painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DotLine));
//                    else
//                        painter->setPen(QPen(QBrush(Qt::lightGray), 1, Qt::DotLine));
//                    QRect br2 = QRect(0, 0, 14, 14);
//                    painter->drawPixmap(QRect(rect.x() + 3 + 6 + row3, rect.y()+(col*3) + 6, 14, 14), QPixmap(":/pic/services.png").scaledToHeight(14, Qt::SmoothTransformation), br);
//                    painter->restore();
//                    mPreviousSelectedDate = date;
//                 }
                 row3 = row3+18;
             }

            else if (i < mAvailableCols*4 && mAvailableRows > 3) {
                painter->drawPixmap(QRect(rect.x() + 3 + row4, rect.y()+(col*4), 16, 16), QPixmap(oprtPixmap(event.oprtType)).scaledToHeight(16, Qt::SmoothTransformation), br);
                 if (!mIsMouseRelease) {
                     mRectsHash.insert(event.actnId, QRect(rect.topLeft().x() + 3 + row4, rect.topLeft().y()+(col*4), 16, 16));
                     mDataHash.insert(event.actnId, event.workName + " - " + event.oprtName);
                     mRectDataHash.insert(event.oprtType, QRect(rect.topLeft().x() + 3 + row4, rect.topLeft().y()+(col*4), 16, 16));
                 }
//                 if (event.actnId == mSelectedActnId) {
//                    painter->save();
//                    if (date == selectedDate())
//                        painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DotLine));
//                    else
//                        painter->setPen(QPen(QBrush(Qt::lightGray), 1, Qt::DotLine));
//                 QRect br2 = QRect(0, 0, 14, 14);
//                 painter->drawPixmap(QRect(rect.x() + 3 + 6 + row4, rect.y()+(col*4) + 6, 14, 14), QPixmap(":/pic/services.png").scaledToHeight(14, Qt::SmoothTransformation), br);
//                 painter->restore();
//                 mPreviousSelectedDate = date;
//                 }
                 row4 = row4+18;
            }
            i++;
         }
    }

	if ((date.dayOfWeek()==Qt::Sunday || date.dayOfWeek()==Qt::Saturday) &&  date.month() == monthShown() && date != selectedDate()) {
		painter->save();
		painter->setPen(Qt::red);
	    painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, QString::number(date.day()));
	    painter->restore();
	    }

	else if (date.month() != monthShown() && date != selectedDate()) {
		painter->save();
		painter->setPen(Qt::lightGray);
		painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, QString::number(date.day()));
		painter->restore();
	}

	else if (date != selectedDate())
		painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, QString::number(date.day()));

}
