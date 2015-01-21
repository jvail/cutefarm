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

#include <QSqlDriver>
#include <QtGui>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QPrinter>
#include <QSqlDatabase>
#include <QDesktopServices>


#include "mainwindow.h"
#include "farmdatadialog.h"
#include "basicdatadialog.h"
#include "settingsdialog.h"
#include "initdbdialog.h"
#include "actionwizard.h"
#include "csvimportdialog.h"

#include "setupwizard/setupwizard.h"
#include "aboutdlg/aboutdlg.h"

#include "cutefarm.h"

#include "actionsviewmodel.h"

#include "helpdock.h"
#include "fieldviewwidget.h"
#include "calendarviewwidget.h"
#include "actionsviewwidget.h"
#include "reportviewwidget.h"



//bool MainWindow::mRunWizard = false;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), mBasicDataDlg(0)
{
	ui.setupUi(this);

	ui.actionCopyAction->setShortcut(QKeySequence::Copy);
	ui.actionDeleteAction->setShortcut(QKeySequence::Delete);
	ui.actionFilter->setShortcut(QKeySequence::Find);
	ui.actionLoadDb->setShortcut(QKeySequence::Open);
	ui.actionPrint->setShortcut(QKeySequence::Print);
	ui.actionRefresh->setShortcut(QKeySequence::Refresh);
	ui.actionAddAction->setShortcut(QKeySequence::New);

	ui.menuPlugins->setVisible(false);
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	updateWindowTitle();
	setupCentralWidget();
	onSettingsChanged();

	fileWatcher = new QFileSystemWatcher(this);
	connect(fileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(watchJournal()));
	updateJournalWatcher();

	readSettings();

    mHelpDock = new HelpDock(this);
    mHelpDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    QAction *act = mHelpDock->toggleViewAction();
    act->setIcon(QIcon(":/pic/help.png"));
    ui.menu_Hilfe->addAction(act);
    ui.toolBarHelp->addAction(act);
    mHelpDock->close();
    addDockWidget(Qt::RightDockWidgetArea, mHelpDock);

    mPrintDoc = new QTextDocument;

    emit databaseChanged();

    if (CuteFarm::mRunWizard) {
        SetupWizard sw(this);
        sw.exec();
    }

    onSettingsChanged();
    fieldViewWidget->onSettingsChanged();
}

MainWindow::~MainWindow()
{
    delete mPrintDoc;
    delete CuteFarm::instance();
}

void MainWindow::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);
    PrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this,  SLOT(onPrinterPaintRequested(QPrinter*)));
    preview.exec();

}

void MainWindow::setupCentralWidget()
{
	calendarViewWidget = new CalendarViewWidget(this);
	fieldViewWidget = new FieldViewWidget(this);
	actionsViewWidget = new ActionsViewWidget(this);
	reportViewWidget = new ReportViewWidget(this);

	mStack = new QStackedWidget(this);
	mStack->insertWidget(View_Actions, actionsViewWidget);
	mStack->insertWidget(View_Calendar, calendarViewWidget);
	mStack->insertWidget(View_Fields, fieldViewWidget);
	mStack->insertWidget(View_Report, reportViewWidget);

    actionsViewWidget->layout()->setContentsMargins(9, 9, 9, 9);
    calendarViewWidget->layout()->setContentsMargins(9, 9, 9, 9);
    fieldViewWidget->layout()->setContentsMargins(9, 9, 9, 9);
    reportViewWidget->layout()->setContentsMargins(9, 9, 9, 9);

	QToolBar *navToolBar = new QToolBar(this);
	navToolBar->setObjectName("navToolBar");
	navToolBar->setWindowTitle(tr("View"));

#ifdef Q_WS_WIN
    QFont f(navToolBar->font());
    f.setPointSize(f.pointSize()+1);
    f.setStyleStrategy(QFont::PreferAntialias);
    navToolBar->setFont(f);
#endif

	navToolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
	navToolBar->setMovable(false);
	navToolBar->setFloatable(false);

	navToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	navToolBar->setIconSize(QSize(48, 48));

	QActionGroup *viewGroup = new QActionGroup(this);
	viewGroup->addAction(ui.actionActionsView);
	viewGroup->addAction(ui.actionCalendarView);
	viewGroup->addAction(ui.actionFieldView);
	viewGroup->addAction(ui.actionReportView);
	viewGroup->setExclusive(true);

    navToolBar->addAction(ui.actionActionsView);
    navToolBar->addAction(ui.actionCalendarView);
    navToolBar->addAction(ui.actionFieldView);
    navToolBar->addAction(ui.actionReportView);

    int size = 90;

#ifdef Q_WS_X11
    size = 110;
#endif

    navToolBar->widgetForAction(ui.actionActionsView)->setFixedSize(size, size);
    navToolBar->widgetForAction(ui.actionCalendarView)->setFixedSize(size, size);
    navToolBar->widgetForAction(ui.actionFieldView)->setFixedSize(size, size);
    navToolBar->widgetForAction(ui.actionReportView)->setFixedSize(size, size);
	addToolBar(Qt::LeftToolBarArea, navToolBar);

	mStack->layout()->setContentsMargins(9, 9, 9, 9);
	setCentralWidget(mStack);

    actionsViewWidget->ui.treeView->setWordWrap(false);
    actionsViewWidget->ui.treeView->header()->setMovable(false);

    mActionsViewModel = new ActionsViewModel(this);
    actionsViewWidget->ui.treeView->setModel(mActionsViewModel);

    mActnTblContextMenu = new QMenu(this);
    mActnTblContextMenu->addAction(ui.actionAddAction);
    mActnTblContextMenu->addAction(ui.actionCopyAction);
    mActnTblContextMenu->addAction(ui.actionEditAction);
    mActnTblContextMenu->addAction(ui.actionDeleteAction);

    connect(fieldViewWidget, SIGNAL(actionTblContextMenuRequested(QPoint)),
            this, SLOT(onActionTblContextMenuRequested(QPoint)));
    connect(calendarViewWidget->ui.calendarWidget, SIGNAL(actionTblContextMenuRequested(QPoint)),
            this, SLOT(onActionTblContextMenuRequested(QPoint)));
    connect(actionsViewWidget->ui.treeView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onTreeViewCustomContextMenuRequested(QPoint)));

    mActnTreeContextMenu = new QMenu(this);
    mActnTreeContextMenu->addAction(ui.actionAddAction);
    mActnTreeContextMenu->addAction(ui.actionCopyAction);
    mActnTreeContextMenu->addAction(ui.actionEditAction);
    mActnTreeContextMenu->addAction(ui.actionDeleteAction);
    mActnTreeContextMenu->addSeparator();

    expandAllAct = new QAction(tr("Expand All"), this);
    connect(expandAllAct, SIGNAL(triggered()), actionsViewWidget, SLOT(onExpandAllTriggered()));
    mActnTreeContextMenu->addAction(expandAllAct);

    collapseAllAct = new QAction(tr("Collapse All"), this);
    connect(collapseAllAct, SIGNAL(triggered()), actionsViewWidget, SLOT(onCollapseAllTriggered()));
    mActnTreeContextMenu->addAction(collapseAllAct);

    connect(ui.actionFilter, SIGNAL(toggled(bool)), actionsViewWidget, SLOT(onActionFilterToggled(bool)));

	connect(calendarViewWidget->ui.calendarWidget, SIGNAL(statusMsg(QString, int)), statusBar(), SLOT(showMessage(QString, int)));
	connect(calendarViewWidget, SIGNAL(statusMsg(QString, int)), statusBar(), SLOT(showMessage(QString, int)));
	connect(reportViewWidget, SIGNAL(statusMsg(QString, int)), statusBar(), SLOT(showMessage(QString, int)));

	connect(this, SIGNAL(databaseChanged()), actionsViewWidget, SLOT(onDatabaseChanged()));
	connect(this, SIGNAL(databaseChanged()), fieldViewWidget, SLOT(onDatabaseChanged()));
	connect(this, SIGNAL(databaseChanged()), reportViewWidget, SLOT(onDatabaseChanged()));

    connect(ui.actionActionsView, SIGNAL(triggered(bool)), this, SLOT(onSwitchStack(bool)));
    connect(ui.actionCalendarView, SIGNAL(triggered(bool)), this, SLOT(onSwitchStack(bool)));
    connect(ui.actionFieldView, SIGNAL(triggered(bool)), this, SLOT(onSwitchStack(bool)));
    connect(ui.actionReportView, SIGNAL(triggered(bool)), this, SLOT(onSwitchStack(bool)));

}

void MainWindow::onPrinterPaintRequested(QPrinter *printer)
{
    mPrintDoc->print(printer);
}

void MainWindow::onSwitchStack(bool show)
{
	QAction *act = qobject_cast<QAction*>(sender());

	if (!act)
		return;

	if (act == ui.actionActionsView) {
		if (show) {
            ui.actionFilter->setEnabled(true);
            ui.actionPrint->setEnabled(true);
            ui.actionCopyAction->setEnabled(true);
            ui.actionDeleteAction->setEnabled(true);
            ui.actionEditAction->setEnabled(true);
			if (mStack->indexOf(actionsViewWidget) >= 0) {
				mStack->setCurrentIndex(mStack->indexOf(actionsViewWidget));
			}
		}
	}
	else if (act == ui.actionCalendarView) {
		if (show) {
            ui.actionFilter->setEnabled(false);
            ui.actionPrint->setEnabled(false);
            ui.actionCopyAction->setEnabled(true);
            ui.actionDeleteAction->setEnabled(true);
            ui.actionEditAction->setEnabled(true);
			if (mStack->indexOf(calendarViewWidget) >= 0) {
				mStack->setCurrentIndex(mStack->indexOf(calendarViewWidget));
			}
		}
	}
	else if (act == ui.actionFieldView) {
		if (show) {
            ui.actionFilter->setEnabled(false);
            ui.actionPrint->setEnabled(false);
            ui.actionCopyAction->setEnabled(true);
            ui.actionDeleteAction->setEnabled(true);
            ui.actionEditAction->setEnabled(true);
			if (mStack->indexOf(fieldViewWidget) >= 0) {
				mStack->setCurrentIndex(mStack->indexOf(fieldViewWidget));
			}
		}
	}
	else if (act == ui.actionReportView) {
		if (show) {
		    ui.actionFilter->setEnabled(false);
		    ui.actionPrint->setEnabled(false);
            ui.actionCopyAction->setEnabled(true);
            ui.actionDeleteAction->setEnabled(true);
            ui.actionEditAction->setEnabled(true);
			if (mStack->indexOf(reportViewWidget) >= 0) {
				mStack->setCurrentIndex(mStack->indexOf(reportViewWidget));
			}
		}
	}
}

void MainWindow::onTreeViewCustomContextMenuRequested(const QPoint &pos)
{
	mActnTreeContextMenu->popup(actionsViewWidget->ui.treeView->viewport()->mapToGlobal(pos));
}
void MainWindow::onActionTblContextMenuRequested(const QPoint &pos)
{
    mActnTblContextMenu->exec(pos);
}

void MainWindow::onSettingsChanged()
{
	bool noLots = SettingsDialog::lotsDisabled();
	fieldViewWidget->ui.lineEditLot->setHidden(noLots);
	if (!noLots && SettingsDialog::locale().endsWith("DE"))
	    ui.actionFildImport->setVisible(true);
	else
	    ui.actionFildImport->setVisible(false);
}

void MainWindow::updateJournalWatcher()
{
	QString path = QSqlDatabase::database().databaseName();
    dbPath = QFileInfo(path).absolutePath();
    dbFileName = QFileInfo(path).fileName();
    if (fileWatcher->files().isEmpty())
        fileWatcher->addPath(dbPath);
    else if (!fileWatcher->files().contains(dbPath)) {
        fileWatcher->removePaths(fileWatcher->files());
        fileWatcher->addPath(dbPath);
    }
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle(QString("CuteFarm %1 - %2 %3")
            .arg(CuteFarm::CUTEFARM_BUILD)
            .arg(FarmDataDialog::farmName())
            .arg(QSqlDatabase::database().databaseName()));
}

void MainWindow::watchJournal()
{
	if(QDir(dbPath).entryList().contains(dbFileName + "-journal")){
		QMessageBox::critical(this, "CuteFarm", "JOURNAL\n"
				"There might be a problem with the database.\n"
				"If there is a 'journal' file next to the db file still existing\n"
				"after restart of CuteFarm please send a bug report.");
	}
}

void MainWindow::on_actionAboutCuteFarm_triggered()
{
    AboutDlg dlg(this);
    dlg.exec();
}

void MainWindow::on_actionBasicData_triggered()
{
    if (!mBasicDataDlg) {
        mBasicDataDlg = new BasicDataDialog(this);
        connect(mBasicDataDlg, SIGNAL(statusMsg(QString, int)), statusBar(), SLOT(showMessage(QString, int)));
    }

    mBasicDataDlg->exec();
}

void MainWindow::on_actionRefresh_triggered()
{
    actionsViewWidget->refresh();
    fieldViewWidget->refresh();
    calendarViewWidget->ui.calendarWidget->updateCalendar();
}

void MainWindow::on_actionFarmData_triggered()
{
	FarmDataDialog fd(this);
	connect(&fd, SIGNAL(statusMsg(QString, int)), statusBar(), SLOT(showMessage(QString, int)));
	fd.exec();
	updateWindowTitle();
}

void MainWindow::on_actionSettings_triggered()
{
	SettingsDialog sd(this);
	connect(&sd, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
	connect(&sd, SIGNAL(settingsChanged()), fieldViewWidget, SLOT(onSettingsChanged()));
	if (mBasicDataDlg)
	    connect(&sd, SIGNAL(settingsChanged()), mBasicDataDlg, SLOT(onSettingsChanged()));
	sd.exec();
}

void MainWindow::on_actionAddAction_triggered()
{
    QDate date = QDate::currentDate();

    switch (mStack->currentIndex()) {
        case View_Calendar: date = calendarViewWidget->selectedDate(); break;
        default: break;
    }

    ActionWizard aw(this, -1, ActionWizard::New, date);
    aw.exec();
}

void MainWindow::on_actionEditAction_triggered()
{
    QModelIndex idx;
    int actnId = -1;

    switch (mStack->currentIndex()) {
        case View_Actions:
            idx = actionsViewWidget->ui.treeView->currentIndex();
            if (idx.isValid())
                actnId = mActionsViewModel->actnId(idx);
            break;
        case View_Fields: {
            foreach (QTableView *view, fieldViewWidget->findChildren<QTableView*>()) {
                if (view->isVisible()) {
                    idx = view->currentIndex();
                    if (idx.isValid())
                        actnId = view->model()->data(idx.sibling(idx.row(), 0)).toInt();
                }
            }
            break;
        }
        case View_Calendar: actnId = calendarViewWidget->ui.calendarWidget->currentActnId(); break;
        default:
            return;
    }

    if (actnId < 0)
        return;

    ActionWizard aw(this, actnId, ActionWizard::Update);
    aw.exec();
}

void MainWindow::on_actionCopyAction_triggered()
{
    QModelIndex idx;
    int actnId = -1;

    switch (mStack->currentIndex()) {
        case View_Actions:
            idx = actionsViewWidget->ui.treeView->currentIndex();
            if (idx.isValid())
                actnId = mActionsViewModel->actnId(idx);
            break;
        case View_Fields: {
            foreach (QTableView *view, fieldViewWidget->findChildren<QTableView*>()) {
                if (view->isVisible()) {
                    idx = view->currentIndex();
                    if (idx.isValid())
                        actnId = view->model()->data(idx.sibling(idx.row(), 0)).toInt();
                }
            }
            break;
        }
        case View_Calendar: actnId = calendarViewWidget->ui.calendarWidget->currentActnId(); break;
        default:
            return;
    }

    if (actnId < 0)
        return;

	ActionWizard aw(this, actnId, ActionWizard::Copy);
	aw.exec();
}

void MainWindow::on_actionDeleteAction_triggered()
{
    QModelIndex idx;
    QString item;
    int actnId = -1;

    switch (mStack->currentIndex()) {
        case View_Actions:
            idx = actionsViewWidget->ui.treeView->currentIndex();
            if (idx.isValid()) {
                item = mActionsViewModel->actnName(idx);
                actnId = mActionsViewModel->actnId(idx);
            }
            break;
        case View_Fields: {
            foreach (QTableView *view, fieldViewWidget->findChildren<QTableView*>()) {
                if (view->isVisible()) {
                    idx = view->currentIndex();
                    if (idx.isValid()) {
                        actnId = view->model()->data(idx.sibling(idx.row(), 0)).toInt();
                        item = view->model()->data(idx.sibling(idx.row(), 1)).toString();
                    }
                }
            }
            break;
        }
        case View_Calendar:
            actnId = calendarViewWidget->ui.calendarWidget->currentActnId();
            item = calendarViewWidget->ui.calendarWidget->currentActnText();
            break;
        default:
            return;
    }

	if (actnId < 0)
		return;

	if (QMessageBox::question(this, "CuteFarm", tr("Delete action '%1'?")
	        .arg(item),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		QSqlDatabase::database().transaction();

		QSqlQuery q;
        q.prepare("DELETE FROM actn WHERE actn_id=:actnId");
        q.bindValue(":actnId", actnId);
        q.exec();
        if (q.lastError().type() != QSqlError::NoError) {
            QSqlDatabase::database().rollback();
            QMessageBox::information(this, "CuteFarm", tr("Error: %1").arg(q.lastError().text()));
            return;
        }
        else {
            if (mStack->currentIndex() == View_Calendar)
                calendarViewWidget->ui.calendarWidget->updateCalendar();
            else if (mStack->currentIndex() == View_Actions)
                mActionsViewModel->deleteActn(idx);
            fieldViewWidget->refreshTables();
        }

		QSqlDatabase::database().commit();
	}
}

void MainWindow::on_actionPrint_triggered()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString header = QString("<p align=\"center\"><span id=\"header\">%1<br/>%2 / %3 %4 - %5</span></p><br/><br/>")
            .arg(FarmDataDialog::farmName())
            .arg(QDate::currentDate().toString(Qt::DefaultLocaleShortDate))
            .arg(tr("Actions:"))
            .arg(actionsViewWidget->startDate().toString(Qt::DefaultLocaleShortDate))
            .arg(actionsViewWidget->endDate().toString(Qt::DefaultLocaleShortDate));
    QString css;
    QFile data(":/css/print.css");
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        css = in.readAll();
    }

    QString html = QString("<html>\n<head>\n<meta content=\"text/html; charset=UTF-8\">\n"
            "<style type=\"text/css\">\n%1\n</style></head>\n<body width=\"100%\">")
            .arg(css);

    html.append(header);
    html.append(actionsViewWidget->htmlTable());
    html.append("</body>\n</html>");
    mPrintDoc->setHtml(html);

    QApplication::restoreOverrideCursor();

    printPreview();

}

void MainWindow::on_actionFildImport_triggered()
{
	QString year = QString::number(QDate::currentDate().year());
	CsvImportDialog id(this, year);
	id.exec();
}

void MainWindow::on_actionNewDb_triggered()
{
    if (CuteFarm::createDatabase(this)) {
        updateWindowTitle();
        emit databaseChanged();
        SetupWizard sw(this);
        sw.exec();
        updateJournalWatcher();
        if (mBasicDataDlg) {
            delete mBasicDataDlg;
            mBasicDataDlg = 0;
        }
    }

    updateWindowTitle();
}

void MainWindow::on_actionLoadDb_triggered()
{
    CuteFarm::openDatabase(this);
    emit databaseChanged();
    updateWindowTitle();
    updateJournalWatcher();
    if (mBasicDataDlg) {
        delete mBasicDataDlg;
        mBasicDataDlg = 0;
    }

    qDebug() << QSqlDatabase::connectionNames();
}

void MainWindow::readSettings()
{
	QSettings s;

	s.beginGroup("Session");
	if (s.contains("Layout")) {
		restoreState(s.value("Layout").toByteArray(), CuteFarm::CUTEFARM_VERSION);
	}
	if (s.contains("Geometry")) {
		restoreGeometry(s.value("Geometry").toByteArray());
	}
	else {
		resize(800, 600);
	}

	s.endGroup();

}

void MainWindow::writeSettings()
{
	QSettings s;
	s.beginGroup("Session");
	s.setValue("Layout", saveState(CuteFarm::CUTEFARM_VERSION));
	s.setValue("Geometry", saveGeometry());
	s.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	writeSettings();
	QMainWindow::closeEvent(event);
}

void MainWindow::loadPlugin()
{

	QString fileName;
	QStringList filter;

#if defined (Q_WS_MAC)
	filter << "*.dylib";
#elif defined (Q_WS_X11)
	filter << "*.so*";
#elif defined (Q_WS_WIN)
	filter << "*.dll";
#endif

	QDir pluginDirectory = QDir(qApp->applicationDirPath());
	//	pluginDirectory.cdUp();
	if (!pluginDirectory.cd("plugins"))
		return;

	//    qDebug() << "lib Path: " << pluginDirectory.path();


	foreach (fileName, pluginDirectory.entryList(filter, QDir::Files)) {
		QPluginLoader loader(pluginDirectory.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();

		//	         qDebug() << "1";

		if (plugin) {

			//	        	 qDebug() << "?" << fileName;

			castPlugin(plugin);
			//	             pluginFileNames += fileName;

		}else{

			QMessageBox::information(this, "Plugin", tr("Could not load plugin.\n")
					+ loader.errorString());
		}
	}

}

void MainWindow::castPlugin(QObject *plugin)
{
	cuteFarmPluginInterface = qobject_cast<CuteFarmPluginInterface *>(plugin);

	ui.menuPlugins->setEnabled(true);
	qDebug() << cuteFarmPluginInterface;

	if (cuteFarmPluginInterface)
	{
		ui.menuPlugins->addAction(cuteFarmPluginInterface->action(this));

		QObject *obj = cuteFarmPluginInterface->actnMonitor(this);
		if (obj)
			connect(this, SIGNAL(fromActnMonitor(const QMultiMap<QString, QString> &)), obj,
					SLOT(actnMonitored(const QMultiMap<QString, QString> &)));
	}
}


