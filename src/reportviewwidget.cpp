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

#include <QDomElement>
#include <QToolButton>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QHeaderView>
#include <QFileDialog>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QColorDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>

#include "reportviewwidget.h"
#include "reportfiledialog.h"
#include "settingsdialog.h"
#include "highlighter.h"
#include "cutefarm.h"


ReportViewWidget::ReportViewWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	setupQry();
}

void ReportViewWidget::onDatabaseChanged()
{
    disconnect(ui.cBTables, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onCbTablesCurrentIndexChanged(QString)));

    ui.cBTables->clear();

    QSqlQuery q;
    q.exec("SELECT tbl_name FROM sqlite_master WHERE type='table' AND tbl_name NOT LIKE '%sqlite%' ORDER BY tbl_name");
    while (q.next())
        ui.cBTables->addItem(q.value(0).toString());

    onCbTablesCurrentIndexChanged(ui.cBTables->currentText());

    connect(ui.cBTables, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onCbTablesCurrentIndexChanged(QString)));

}

void ReportViewWidget::onCbTablesCurrentIndexChanged(const QString &table)
{
    if (table.isEmpty())
        return;

    QSqlQuery q;
    q.exec(QString("SELECT sql FROM sqlite_master WHERE type='table' AND tbl_name='%1'").arg(table));
    while (q.next())
        ui.textEditSchema->setText(q.value(0).toString());
}

void ReportViewWidget::setupQry()
{
	m_currentQry = QString();

	QWidget *cornerWidget = new QWidget(this);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 1);
	layout->setSpacing(1);
	cornerWidget->setLayout(layout);

	m_actnQryButton = new QToolButton(this);
	m_actnQryButton->setMinimumSize(20, 20);
	m_actnQryButton->setIcon(QIcon(":/pic/kchart_chrt.png"));
	m_actnQryButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_actnQryButton->setPopupMode(QToolButton::InstantPopup);
	layout->addWidget(m_actnQryButton);
	QMenu *menu = new QMenu(this);
	menu->addAction(ui.actionRunQryFromText);
	menu->addAction(ui.actionSaveQry);
	menu->addAction(ui.actionCloseQry);
	menu->addSeparator();
	menu->addAction(ui.actionCSV);
	m_actnQryButton->setMenu(menu);

	ui.tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

	m_qryModel = new SqlQueryModel(this);
	QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
	proxy->setSourceModel(m_qryModel);
	ui.resultView->setModel(proxy);

	m_dirModel = new QDirModel(QStringList() /*<< "*.qry"*/, QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot,
			QDir::Name, this);

	m_dirModel->setReadOnly(false);

	FileIconProvider *fIp= new FileIconProvider;
	m_dirModel->setIconProvider(fIp);

//	if (!QDir(QDir::home().path() + "/CuteFarm/Reports").exists()) {
//		QDir(QDir::home().path()).mkdir("CuteFarm");
//		QDir(QDir::home().path() + "/CuteFarm").mkdir("Reports");
//	}
//
//	qDebug() << QDir::home().path();

	ui.dirView->setModel(m_dirModel);
	ui.dirView->setRootIndex(m_dirModel->index(CuteFarm::reportsDirPath()));

	ui.dirView->setAcceptDrops(true);
	ui.dirView->hideColumn(1);
	ui.dirView->hideColumn(2);
	ui.dirView->hideColumn(3);

	m_treeWidgetQryStrgFileActns << ui.actionEditQry << ui.actionDeleteQry << ui.actionRunQryFromFile << ui.actionNewFolder;
	m_treeWidgetQryStrgDirActns << ui.actionNewFolder << ui.actionDeleteFolder;

	ui.dirView->resizeColumnToContents(0);
	ui.dirView->header()->setResizeMode(0, QHeaderView::ResizeToContents);

	ui.resultView->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.resultView->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.resultView->setShowGrid(SettingsDialog::showGrid());
//	resultView->horizontalHeader()->setStretchLastSection(true);

	Highlighter *highlighter = new Highlighter(0, true);
    highlighter->setDocument(ui.textEditSql->document());

    Highlighter *schemaHighlighter = new Highlighter(0, false);
    schemaHighlighter->setDocument(ui.textEditSchema->document());

}

void ReportViewWidget::on_dirView_customContextMenuRequested(const QPoint &pos)
{
	QString path = m_dirModel->data(ui.dirView->indexAt(pos), QDirModel::FilePathRole).toString();

	if (QFileInfo(path).isDir()) {
		if (QDir(path).exists()) {
		    qDebug() << QDir(path).entryList();
		    if (!QDir(path).entryList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs).isEmpty())
		        ui.actionDeleteFolder->setEnabled(false);
		    else
		        ui.actionDeleteFolder->setEnabled(true);

			m_menuTreeWidgetQryStrg.exec(m_treeWidgetQryStrgDirActns, QCursor::pos());
		}
		else
			return;
	}
	else if (QFileInfo(path).isFile()) {
		if (QFile(path).exists()) {
			qDebug() << "file";
			m_menuTreeWidgetQryStrg.exec(m_treeWidgetQryStrgFileActns, QCursor::pos());
		}
		else
			return;
	}
	else if (!ui.dirView->indexAt(pos).isValid()) {
		m_menuTreeWidgetQryStrg.exec(m_treeWidgetQryStrgDirActns, QCursor::pos());
	}
}

void ReportViewWidget::on_actionCSV_triggered()
{
	if (m_qryModel->rowCount() < 1)
		return;

	QString fileName = QFileDialog::getSaveFileName(
			this, tr("Save csv file"), QDir::homePath(), "*.csv");

	if (fileName.isEmpty())
		return;

	QFile file(fileName);

	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
	    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QTextStream out(&file);
		out.setCodec("UTF-8");
        for (int col = 0; col < m_qryModel->columnCount(); ++col) {
            out << m_qryModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
            out << "|";

        }
        out << endl;
		for (int i = 0; i < m_qryModel->rowCount(); ++i) {
			for (int j = 0; j < m_qryModel->columnCount(); ++j) {
				out << m_qryModel->record(i).field(j).value().toString();
				out << "|";
			}
			out << endl;
		}
		emit statusMsg(tr("Saved"), 5000);
		file.close();
		QApplication::restoreOverrideCursor();
	}
	else
		emit statusMsg(tr("Error: %1").arg(file.errorString()), 5000);

}

void ReportViewWidget::on_actionEditQry_triggered()
{
	m_currentQry.clear();
	ui.textEditSql->clear();
	ui.textEditComt->clear();
	m_actnQryButton->setText("");

	QString path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();
	if (!QFileInfo(path).isFile() || !QFile(path).exists())
		return;

    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "CuteFarm",
                             tr("Can not read file %1\nError: %2")
                             .arg(QFile(path).fileName())
                             .arg(file.errorString()));
        return;
    }

    if (!m_domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(this, "CuteFarm",
                                 tr("Error in line %1, column %2\nError: %3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return;
    }

    QDomElement root = m_domDocument.documentElement();
    if (root.tagName() != "qry") {
        QMessageBox::information(this, "CuteFarm",
                                 tr("This file is not a query file."));
        return;
    } else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0") {
        QMessageBox::information(this, "CuteFarm",
                                 tr("The file is not an query version 1.0 "
                                    "file."));
        return;
    }

    QString desc = root.firstChildElement("description").text();
    QString sql = root.firstChildElement("sql").text();

    ui.textEditSql->setText(sql);
    ui.textEditComt->setText(desc);
    m_actnQryButton->setText(QFileInfo(path).fileName());

    file.close();
    m_currentQry = path;
}

void ReportViewWidget::on_actionDeleteQry_triggered()
{
	QString path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();
	if (QFileInfo(path).isFile() && QFile(path).exists()) {
	    QModelIndex parent = ui.dirView->currentIndex().parent();
	    if (QMessageBox::question(this, "CuteFarm", tr("Delete file '%1'").arg(path), QMessageBox::Yes|QMessageBox::No)
	        == QMessageBox::Yes) {
		if(QFile(path).remove())
			m_dirModel->refresh(parent);
	    }
		return;
	}
}

void ReportViewWidget::on_actionSaveQry_triggered()
{
	if (m_currentQry.isEmpty()) {

		ReportFileDialog fD(m_dirModel, this);
		start:
	    if (fD.exec() != QDialog::Accepted)
	         return;

		if (fD.fileName().isEmpty())
			return;

		m_currentQry = fD.path() + "/" + fD.fileName();

		if (QFile(m_currentQry).exists()) {
	        if (QMessageBox::question(this, "CuteFarm",
	                             tr("File %1 exists.\nReplace?")
	                             .arg(m_currentQry), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
	        	goto start;
		}

		QFile file(m_currentQry);
	    if (!file.open(QFile::WriteOnly | QFile::Text)) {
	        QMessageBox::warning(this, "CuteFarm",
	                             tr("Can not save file %1\n%2.")
	                             .arg(file.fileName())
	                             .arg(file.errorString()));
	        return;
	    }

	    QTextStream out(&file);
	    out << "<?xml version='1.0' encoding='UTF-8'?>\n"
	    	   "<!DOCTYPE qry>\n"
	    	   "<qry version=\"1.0\">\n"
	    	   "	<description>"+ Qt::escape(ui.textEditComt->toPlainText()) +"</description>\n"
	    	   "	<sql>"+ Qt::escape(ui.textEditSql->toPlainText()) +"</sql>\n"
	    	   "</qry>\n";

	    QModelIndex parent = ui.dirView->currentIndex().parent();
	    m_dirModel->refresh(parent);
	    file.close();
	    m_actnQryButton->setText(QFileInfo(file.fileName()).fileName());

	    return;
	}

    QFile file(m_currentQry);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "CuteFarm",
                             tr("Can not save file %1\n%2.")
                             .arg(QFile(m_currentQry).fileName())
                             .arg(file.errorString()));
        return;
    }

    QDomElement root = m_domDocument.documentElement();
    QDomElement oldDesc = root.firstChildElement("description");
    QDomElement newDesc = m_domDocument.createElement("description");
    QDomText newDescText = m_domDocument.createTextNode(ui.textEditComt->toPlainText());
    newDesc.appendChild(newDescText);
    root.replaceChild(newDesc, oldDesc);
    QDomElement oldSql = root.firstChildElement("sql");
    QDomElement newSql = m_domDocument.createElement("sql");
    QDomText newSqlText = m_domDocument.createTextNode(ui.textEditSql->toPlainText());
    newSql.appendChild(newSqlText);
    root.replaceChild(newSql, oldSql);

    const int IndentSize = 4;

    QTextStream out(&file);
    m_domDocument.save(out, IndentSize);
    file.close();
    QModelIndex parent = ui.dirView->currentIndex().parent();
    m_dirModel->refresh(parent);

    emit statusMsg(tr("Saved: %1").arg(m_currentQry), 5000);
}

void ReportViewWidget::on_actionCloseQry_triggered()
{
	m_currentQry.clear();
	m_actnQryButton->setText(QString());
	ui.textEditComt->clear();
	ui.textEditError->clear();
	ui.textEditSql->clear();
}

void ReportViewWidget::on_dirView_doubleClicked(const QModelIndex &index)
{
	if (!index.isValid())
		return;

	if (QFileInfo(m_dirModel->data(index, QDirModel::FilePathRole).toString()).isDir())
		return;

	ui.dirView->setCurrentIndex(index);

	on_actionRunQryFromFile_triggered();
}

void ReportViewWidget::on_actionRunQryFromFile_triggered()
{
	QString path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();
	if (!QFileInfo(path).isFile() || !QFile(path).exists())
		return;

    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "CuteFarm",
                             tr("Can not read file  %1\nError: %2")
                             .arg(QFile(path).fileName())
                             .arg(file.errorString()));
        return;
    }

    if (!m_domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(this, "CuteFarm",
                                 tr("Error in line %1, column %2\nError: %3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return;
    }

    QDomElement root = m_domDocument.documentElement();
    if (root.tagName() != "qry") {
        QMessageBox::information(this, "CuteFarm",
                                 tr("This file is not a query file."));
        return;
    } else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0") {
        QMessageBox::information(this, "CuteFarm",
                                 tr("The file is not an query version 1.0 "
                                    "file."));
        return;
    }

    QString sql = root.firstChildElement("sql").text();
    file.close();

	if (sql.contains("INSERT", Qt::CaseInsensitive) ||
			sql.contains("UPDATE", Qt::CaseInsensitive) ||
			sql.contains("CREATE", Qt::CaseInsensitive) ||
			sql.contains("DELETE", Qt::CaseInsensitive))
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_qryModel->setQuery(sql);
	ui.resultView->resizeColumnsToContents();
	ui.resultView->horizontalHeader()->setStretchLastSection(true);
	QApplication::restoreOverrideCursor();

	if (m_qryModel->lastError().type() != QSqlError::NoError)
		emit statusMsg(tr("Error"), 2500);
	else
		emit statusMsg("Ok", 2500);

	ui.textEditError->setText(m_qryModel->lastError().text());
}

void ReportViewWidget::on_actionNewQry_triggered()
{
	QString path;

	if (QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).isDir())
		path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();
	else if (QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).isFile())
		path = QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).absolutePath();
	else if (!ui.dirView->currentIndex().isValid()) {
		path = m_dirModel->data(ui.dirView->rootIndex(), QDirModel::FilePathRole).toString();
		qDebug() << path;
	}
	else
		return;

	if (QFile(path + QDir::separator() + tr("New Query")).exists()) {
        if (QMessageBox::question(this, "CuteFarm",
                             tr("File %1 exists.\nReplace?")
                             .arg(path + QDir::separator() + tr("New Query")), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        	return;
	}
	qDebug() << path;
    QFile file(path + QDir::separator() + tr("New Query"));
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "CuteFarm",
                             tr("Can not save file %1\nError: %2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << "<?xml version='1.0' encoding='UTF-8'?>\n"
    	   "<!DOCTYPE qry>\n"
    	   "<qry version=\"1.0\">\n"
    	   "	<description></description>\n"
    	   "	<sql></sql>\n"
    	   "</qry>\n";

    m_dirModel->refresh(m_dirModel->index(path, 0));
    file.close();

}

void ReportViewWidget::on_actionNewFolder_triggered()
{
	QString path;

	if (QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).isDir())
		path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();
	else if (QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).isFile())
		path = QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).absolutePath();
	else if (!ui.dirView->currentIndex().isValid()) {
		path = m_dirModel->data(ui.dirView->rootIndex(), QDirModel::FilePathRole).toString();
	}

	bool ok;
	QString name = QInputDialog::getText(this, tr("New Folder"), tr("Folder Name:"), QLineEdit::Normal, QString(), &ok);

	if (!ok || name.trimmed().isEmpty())
	    return;

	m_dirModel->mkdir(m_dirModel->index(path, 0), name);
}

void ReportViewWidget::on_actionDeleteFolder_triggered()
{
    QString path;

    if (QFileInfo(m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString()).isDir())
        path = m_dirModel->data(ui.dirView->currentIndex(), QDirModel::FilePathRole).toString();

    if (path.isEmpty())
        return;

    QModelIndex parent = ui.dirView->currentIndex().parent();

//    QDir::rmdir(path);
    if (QMessageBox::question(this, "CuteFarm", tr("Delete folder '%1'").arg(path), QMessageBox::Yes|QMessageBox::No)
        == QMessageBox::Yes)
        QDir(path).rmpath(path);


    m_dirModel->refresh(parent);

}

void ReportViewWidget::on_actionRunQryFromText_triggered()
{
	QString query = ui.textEditSql->toPlainText();
	if (query.isEmpty())
		return;

	if (query.contains("INSERT", Qt::CaseInsensitive) ||
			query.contains("UPDATE", Qt::CaseInsensitive) ||
			query.contains("CREATE", Qt::CaseInsensitive) ||
			query.contains("DELETE", Qt::CaseInsensitive) ||
			query.contains("PRAGMA", Qt::CaseInsensitive))
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_qryModel->setQuery(query);
	ui.resultView->resizeColumnsToContents();
	ui.resultView->horizontalHeader()->setStretchLastSection(true);
	QApplication::restoreOverrideCursor();

	if (m_qryModel->lastError().type() != QSqlError::NoError)
		emit statusMsg(tr("Error"), 2500);
	else
		emit statusMsg("Ok", 2500);

	ui.textEditError->setText(m_qryModel->lastError().text());

}

ReportViewWidget::~ReportViewWidget()
{

}
