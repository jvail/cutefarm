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


#ifndef MainWindow_H
#define MainWindow_H

#include <QPrintPreviewDialog>
#include <QHBoxLayout>
#include <QToolButton>

class HelpDock;
class ActionsViewWidget;
class CalendarViewWidget;
class FieldViewWidget;
class ReportViewWidget;
class ActionsViewModel;
class BasicDataDialog;

class QStackedWidget;
class QAction;
class QFileSystemWatcher;
class QCloseEvent;
class QLabel;
class QTextDocument;
class QPrinter;

#include "ui_mainwindow.h"
#include "cutefarmplugininterface.h"

class PrintPreviewDialog : public QPrintPreviewDialog
{
    Q_OBJECT

public:
    PrintPreviewDialog(QPrinter *printer, QWidget *parent = 0)
    : QPrintPreviewDialog(printer, parent)
    {
        setObjectName("printPreviewDialog");

        setSizeGripEnabled(true);

        QList<QToolBar*> tool = findChildren<QToolBar *>();
        if (!tool.isEmpty())
            tool.first()->setHidden(true);

        QWidget *w = new QWidget(this);
        QHBoxLayout *hLay = new QHBoxLayout;
        hLay->setContentsMargins(0, 0, 0, 0);

        QToolButton *zoomInTool = new QToolButton;
        zoomInTool->setIcon(QIcon(":/pic/viewmag+32.png"));
        zoomInTool->setIconSize(QSize(32, 32));
        zoomInTool->setFixedSize(QSize(40, 40));

        QToolButton *zoomOutTool = new QToolButton;
        zoomOutTool->setIcon(QIcon(":/pic/viewmag-32.png"));
        zoomOutTool->setIconSize(QSize(32, 32));
        zoomOutTool->setFixedSize(QSize(40, 40));

        QToolButton *printTool = new QToolButton;
        printTool->setIcon(QIcon(":/pic/fileprint.png"));
        printTool->setIconSize(QSize(32, 32));
        printTool->setFixedSize(QSize(40, 40));

        QToolButton *pageSetupTool = new QToolButton;
        pageSetupTool->setIcon(QIcon(":/pic/configure.png"));
        pageSetupTool->setIconSize(QSize(32, 32));
        pageSetupTool->setFixedSize(QSize(40, 40));

        QToolButton *abortTool = new QToolButton;
        abortTool->setIcon(QIcon(":/pic/cancel-32.png"));
        abortTool->setIconSize(QSize(32, 32));
        abortTool->setFixedSize(QSize(40, 40));

        hLay->addWidget(zoomInTool);
        hLay->addWidget(zoomOutTool);
        hLay->insertStretch(2);
        hLay->addWidget(printTool);
        hLay->addWidget(pageSetupTool);
        hLay->addWidget(abortTool);

        w->setLayout(hLay);

        QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(layout());
        lay->setContentsMargins(9, 9, 9, 9);
        lay->insertWidget(0, w);

        connect(zoomInTool, SIGNAL(clicked()), this, SLOT(_q_zoomIn()));
        connect(zoomOutTool, SIGNAL(clicked()), this, SLOT(_q_zoomOut()));
        connect(printTool, SIGNAL(clicked()), this, SLOT(_q_print()));
        connect(pageSetupTool, SIGNAL(clicked()), this, SLOT(_q_pageSetup()));
        connect(abortTool, SIGNAL(clicked()), this, SLOT(reject()));

    }
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum ViewIndex { View_Actions = 0, View_Calendar = 1, View_Fields = 2, View_Report = 3 };

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void databaseChanged();

private slots:
    void onPrinterPaintRequested(QPrinter *printer);
	void onSwitchStack(bool show);

	//file
	void on_actionNewDb_triggered();
	void on_actionLoadDb_triggered();
	void on_actionFildImport_triggered();
	void on_actionPrint_triggered();

	//edit
	void on_actionAddAction_triggered();
	void on_actionDeleteAction_triggered();
	void on_actionEditAction_triggered();
	void on_actionCopyAction_triggered();
	void on_actionSettings_triggered();

	//data
	void on_actionFarmData_triggered();
	void on_actionBasicData_triggered();
	void on_actionRefresh_triggered();

	//help
	void on_actionAboutCuteFarm_triggered();

	void watchJournal();

	void loadPlugin();

	void onTreeViewCustomContextMenuRequested(const QPoint &pos);
	void onActionTblContextMenuRequested(const QPoint &pos);
	void onSettingsChanged();

private:
	void printPreview();
	void setupCentralWidget();
    void updateJournalWatcher();
    void updateWindowTitle();

    void readSettings();
    void writeSettings();

    void castPlugin(QObject *plugin);

    CuteFarmPluginInterface *cuteFarmPluginInterface;

    QStackedWidget *mStack;
	ActionsViewWidget *actionsViewWidget;
	FieldViewWidget *fieldViewWidget;
	CalendarViewWidget *calendarViewWidget;
	ReportViewWidget *reportViewWidget;

	ActionsViewModel *mActionsViewModel;

	QString dbPath;
	QString dbFileName;
    QFileSystemWatcher *fileWatcher;

    QMenu *mActnTblContextMenu;
	QMenu *mActnTreeContextMenu;
	QAction *expandAllAct;
	QAction *collapseAllAct;

	HelpDock *mHelpDock;
	BasicDataDialog *mBasicDataDlg;

	QTextDocument *mPrintDoc;

    Ui::MainWindowClass ui;
};

#endif // MainWindow_H
