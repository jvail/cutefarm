/* 	CuteFarm
    Copyright (C) 2008-2011  Jan Vaillant

    functions for coordinates to/from display are from QMapControl
    Copyright (C) 2007 - 2008 Kai Winter

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

#ifndef REPORTVIEWWIDGET_H
#define REPORTVIEWWIDGET_H

#include <QtGui/QWidget>
#include <QDirModel>
#include <QDebug>
#include <QMenu>
#include <QLabel>
#include <QDomDocument>
#include <QSqlQueryModel>
#include <QMimeData>
#include <QList>
#include <QUrl>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QToolButton>

#include "ui_reportviewwidget.h"
#include "sqlquerymodel.h"




class FileIconProvider : public QFileIconProvider {

	public:

	FileIconProvider() : QFileIconProvider()
	{}

	QIcon icon(IconType type) const
	{
		qDebug() << "type" << type;

		if (type == QFileIconProvider::Folder)
			return QIcon(":/pic/folder_blue_open.png");

		if (type == QFileIconProvider::File)
			return QIcon(":/pic/kchart_chrt.png");

		else
			return QFileIconProvider::icon(type);
	}

	QIcon icon(const QFileInfo &info) const
	{
		if (info.isDir()) {

			if (QDir(info.absoluteFilePath()).entryInfoList(QStringList() /*<< "*.qry"*/, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
			QDir::Name).isEmpty())
				return QIcon(":/pic/folder_blue.png");
			else
				return QIcon(":/pic/folder_blue_open.png");
		}

		if (info.isFile())
			return QIcon(":/pic/kchart_chrt.png");

		else
			return QFileIconProvider::icon(info);
	}

	QString type(const QFileInfo &info) const
	{
//		qDebug() << info;
		return QFileIconProvider::type(info);
	}

};

class ReportViewWidget : public QWidget
{
    Q_OBJECT

public:

    ReportViewWidget(QWidget *parent = 0);
    ~ReportViewWidget();

    Ui::ReportViewWidgetClass ui;

signals:
	void statusMsg(QString, int);

public slots:
    void onDatabaseChanged();

private slots:
    void onCbTablesCurrentIndexChanged(const QString &table);
	void setupQry();
	void on_dirView_customContextMenuRequested(const QPoint &pos);

	void on_actionCSV_triggered();
	void on_actionEditQry_triggered();
	void on_actionDeleteQry_triggered();
	void on_actionSaveQry_triggered();
	void on_actionCloseQry_triggered();
	void on_actionRunQryFromFile_triggered();
	void on_dirView_doubleClicked(const QModelIndex &index);
	void on_actionRunQryFromText_triggered();
	void on_actionNewQry_triggered();
	void on_actionNewFolder_triggered();
	void on_actionDeleteFolder_triggered();

private:

	QDirModel *m_dirModel;

	QToolButton *m_actnQryButton;

	QMenu m_menuTreeWidgetQryStrg;
	QList<QAction*> m_treeWidgetQryStrgDirActns;
	QList<QAction*> m_treeWidgetQryStrgFileActns;

	QDomDocument m_domDocument;
	QString m_currentQry;

	SqlQueryModel *m_qryModel;

	QGraphicsScene *m_scene;

	QMenu m_menuTreeWidgetShpStrg;

	qreal PI;


	QWidget *m_scaleWidget;
	QLabel *m_coordLabel;
	QGraphicsLineItem *m_distanceItem;

	bool m_leftButtonPressed;
	QPointF m_firstPoint;
	QPointF m_secondPoint;

};

#endif // REPORTVIEWWIDGET_H
