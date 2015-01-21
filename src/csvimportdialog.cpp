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

#include <QTextEdit>

#include "csvimportdialog.h"

CsvImportDialog::CsvImportDialog(QWidget *parent, const QString &yearOfHarvest)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint)
{
	ui.setupUi(this);

	QVariant w = height() * 1.33;
	resize(w.toInt(), height());

	qm = new QSqlTableModel(this);
	ui.dateEdit->setDate(QDate::fromString(yearOfHarvest, "yyyy"));
	ui.tableView->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.progressBar->hide();

	QList<QByteArray> list = QTextCodec::availableCodecs();
	QStringList codecList;
    for (int i = 0; i < list.size(); ++i) {
    	codecList << list.at(i);
    }
    codecList.sort();
	ui.comboBoxCodec->addItems(codecList);
	ui.comboBoxCodec->setCurrentIndex(codecList.indexOf("UTF-8"));

#ifdef Q_WS_WIN
	ui.comboBoxCodec->setCurrentIndex(codecList.indexOf("System"));
#endif

	ui.fileLineEdit->setText(":/csv/example_test_utf8.csv");

	on_pushButtonHelp_clicked();
}

void CsvImportDialog::on_pushButtonHelp_clicked()
{
	QMessageBox::information(this->parentWidget(), "CuteFarm", "Der Import von Schlägen und Flurstücken wird im Moment nur für csv Exporte aus <b>FIONA</b> "
			" (Flächeninformationssystem für Baden-Württemberg) und sollte nur <b>einmalig</b> zu Beginn der "
			"Programmnutzung erfolgen. Sie benötigen eine sog. csv Datei, um die Daten importieren zu können. Das "
			"Trennungzeichen muss ein <b>Semikolon</b> (;) sein.<br><br>"
			"<b>Vorgehen</b>:"
			"<br>Betriebnamen ohne Leerzeichen angeben und Erntejahr auswählen.<br><br> Wählen Sie mit dem Dateidialog die richtige Datei aus. <br>Setzen Sie die Kodierung auf die Kodierung der csv Datei "
			"sonst werden z.B. Umlaute nicht korrekt übernommen.<br>'Laden' Sie die Datei und prüfen Sie, ob alles korrekt angezeigt wird.<br> "
			"Wählen Sie das Erntejahr für die zu importierenden Flächeninformationen.<br>"
			"Unter Schlagname kann die Kulturart ausgewählt werden.<br>"
			"Aktivieren Sie die Spalten und wählen Sie den passenden Eintrag (Nr.-Spalten dürfen nur Zahlen enthalten). "
			"Keine Spalte darf mehr als einmal ausgewählt werden."
//			"Sollten die Schlagnamen nicht in der Datei enthalten sein, wird automatisch ein Schlagname erzeugt. "
//			"Diesen können Sie später über Betriebsdaten -> Flächen ändern.<br>"
			"<br>'Importieren' Sie die Flächeninformationen.");
}

void CsvImportDialog::on_filePushButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
					     "CSV", QDir::homePath(), "CSV (*.csv)");
	if (!path.isEmpty())
			ui.fileLineEdit->setText(path);
}

bool CsvImportDialog::selectionIsUnique()
{
	QStringList list;
	list <<	ui.comboBoxCropCodeNr->currentText();
	list << ui.comboBoxFieldLotSize->currentText();
	list << ui.comboBoxFieldName->currentText();
	list << ui.comboBoxFieldNr->currentText();
	list << ui.comboBoxGemark->currentText();
	list << ui.comboBoxLotNr->currentText();
	list << ui.comboBoxLotSize->currentText();
	list << ui.comboBoxFlur->currentText();
	list << ui.comboBoxLotSubNr->currentText();
	if(
			(list.count(ui.comboBoxFieldLotSize->currentText()) > 1 && ui.checkBoxFieldLotSize->checkState() == Qt::Checked) ||
			(list.count(ui.comboBoxFieldName->currentText()) > 1 && ui.checkBoxFieldName->checkState() == Qt::Checked) ||
			(list.count(ui.comboBoxFieldNr->currentText()) > 1 && ui.checkBoxFieldNr->checkState() == Qt::Checked)||
			(list.count(ui.comboBoxGemark->currentText()) > 1 && ui.checkBoxGemark->checkState() == Qt::Checked)||
			(list.count(ui.comboBoxLotNr->currentText()) > 1 && ui.checkBoxLotNr->checkState() == Qt::Checked)||
			(list.count(ui.comboBoxLotSubNr->currentText()) > 1 && ui.checkBoxLotSubNr->checkState() == Qt::Checked)||
			(list.count(ui.comboBoxFlur->currentText()) > 1 && ui.checkBoxFlur->checkState() == Qt::Checked)){

		return false;
	}
	else
		return true;
}

void CsvImportDialog::on_pushButtonLoad_clicked()
{
	if(ui.fileLineEdit->text().isEmpty()){
		return;
	}
	QString fileName = ui.fileLineEdit->text();
	ui.progressBar->setValue(0);
	ui.progressBar->show();
//	qm->removeColumns(0, qm->columnCount(QModelIndex()), QModelIndex());
	QSqlQuery q;
	q.exec("SELECT tbl_name FROM (SELECT tbl_name FROM sqlite_temp_master "
			"UNION "
			"SELECT tbl_name FROM sqlite_master) WHERE tbl_name='csv'");
	while(q.next()){
		if(q.value(0).isValid()){
			delete qm;
			q.clear();
			qm = new QSqlTableModel(this);
			QSqlQuery qDrop("DROP TABLE csv");
			qDebug() << qDrop.lastQuery() << qDrop.lastError().text();
		}
	}


	QFile field(fileName);
	QString line;
	if (field.open(QFile::ReadOnly)){
		QTextStream in(&field);
		QTextCodec *codec = QTextCodec::codecForName(ui.comboBoxCodec->currentText().toAscii());
//		QString codec = ui.comboBoxCodec->currentText();
		in.setCodec(codec);
//		in.setCodec("UTF-8");

		int row = 0;
		while (!in.atEnd()){
			line = in.readLine();
			if(row == 0){
				QString strQuery;
				QString part;
				if(QSqlDatabase::database().databaseName() == ":memory:")
					strQuery.append("CREATE TABLE csv ("); //nicht :memory: -> strQuery.append("CREATE TEMP TABLE csv (");
				else
					strQuery.append("CREATE TEMP TABLE csv ("); //nicht :memory: -> strQuery.append("CREATE TEMP TABLE csv (");

				QStringList list = line.split(ui.comboBoxSep->currentText());

			     for (int i = 0; i < list.size() -1; ++i){
//			    	 part = list.at(i).toLocal8Bit().constData();
			    	 part = list.at(i).simplified();//.toLocal8Bit().constData();
			    	 part.prepend("'").append("'");
			    	 part.replace(" ", "_");
			    	 strQuery.append(part);
			    	 strQuery.append(" TEXT,");
			     }
			     strQuery.chop(1);
			     strQuery.append(", fild_use_id, year_of_harvest ");
			     strQuery.append(")");
			     strQuery.replace(".", "_");
			     strQuery.replace("-", "_");

			     QSqlQuery q(strQuery);
//			     qDebug() << q.lastQuery();
				 row++;
			     continue;
			}
			row++;
			ui.progressBar->setValue(row);
			QString strQuery;
			QString part;
			strQuery.append("INSERT INTO csv VALUES(");


			QStringList list = line.split(ui.comboBoxSep->currentText());

		     for (int i = 0; i < list.size() -1; ++i){
		    	 part = list.at(i).toLocal8Bit().constData();
		    	 part.prepend("'").append("',");
		    	 strQuery.append(part);
		     }
		     if(part.isEmpty())
		    	 continue;
		     strQuery.chop(1);
		     strQuery.append(", 1, " + QString::number(ui.dateEdit->date().year()));
		     strQuery.append(")");

		     QSqlQuery q(strQuery);
//		     qDebug() << q.lastQuery();
//		     qDebug() << q.lastError().text();


		}
	}
	else{
		QMessageBox::information(this, "CuteFarm", "Keine gültige Datei.");
		return;
	}

	field.close();
	qm->setTable("csv");
	qm->setEditStrategy(QSqlTableModel::OnManualSubmit);
	qm->select();
	while (qm->canFetchMore())
		qm->fetchMore();

	ui.progressBar->setValue(99);
	ui.tableView->setModel(qm);
	ui.tableView->resizeColumnsToContents();

	list.clear();
	ui.comboBoxCropCodeNr->clear();
	ui.comboBoxFieldLotSize->clear();
	ui.comboBoxFieldName->clear();
	ui.comboBoxFieldNr->clear();
	ui.comboBoxGemark->clear();
	ui.comboBoxLotNr->clear();
	ui.comboBoxLotSubNr->clear();
	ui.comboBoxLotSize->clear();
	ui.comboBoxFlur->clear();

	for (int i = 0; i < qm->columnCount(QModelIndex()); ++i){
		list << qm->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
	}
	list.removeLast();
	list.removeLast();
	ui.progressBar->setValue(100);

	ui.comboBoxCropCodeNr->addItems(list);
	ui.comboBoxFieldLotSize->addItems(list);
	ui.comboBoxFieldName->addItems(list);
	ui.comboBoxFieldNr->addItems(list);
	ui.comboBoxGemark->addItems(list);
	ui.comboBoxLotNr->addItems(list);
	ui.comboBoxLotSubNr->addItems(list);
	ui.comboBoxLotSize->addItems(list);
	ui.comboBoxFlur->addItems(list);
	ui.progressBar->hide();

}

void CsvImportDialog::on_pushButtonOk_clicked()
{
	check();

	QSqlQuery q;

	if (ui.lineEditUserName->text().isEmpty())
		return;

	q.exec("INSERT INTO user (user_id, user_name) VALUES(NULL,'"+ui.lineEditUserName->text().simplified()+"')");

	if (q.lastError().type() != QSqlError::NoError) {
		qDebug() << q.lastError().text() << q.lastQuery();
		return;
	}

	QString userId;
	q.exec("SELECT user_id FROM user WHERE user_name='"+ui.lineEditUserName->text().simplified()+"'");
	while (q.next())
		userId = q.value(0).toString();


	if (ui.checkBoxFieldLotSize->isChecked() == false ||
			ui.checkBoxFieldName->isChecked() == false ||
			ui.checkBoxFieldNr->isChecked() == false ||
			ui.checkBoxLotNr->isChecked() == false ||
			ui.checkBoxGemark->isChecked() == false ||
			ui.checkBoxLotSubNr->isChecked() == false) {
		QMessageBox::information(this, "Fehler", "Alle Felder bis auf 'Flur' müssen ausgewählt sein.");
		return;
	}

	if (!selectionIsUnique()) {
		QMessageBox::information(this, "Fehler", "Spalten nicht mehrfach auswählen!");
		return;
	}

	ui.progressBar->show();

	QString insertLot = "";
	QString selectLot = "";
	QString insertFild = "";
	QString selectFild = "";

	ui.progressBar->setValue(10);

	if(ui.checkBoxFieldName->isChecked() &&
			!ui.comboBoxFieldName->currentText().isEmpty()){
		insertFild.append(", user_id, fild_name");
		if(ui.comboBoxFieldName->currentText() == "Kulturart")
			selectFild.append(", "+ userId  + ", " + ui.comboBoxFieldName->currentText() +" || "+ui.comboBoxFieldNr->currentText());
		else
			selectFild.append(", "+ userId  + ", " + ui.comboBoxFieldName->currentText());
	}
	if(ui.checkBoxFieldNr->isChecked() &&
			!ui.comboBoxFieldNr->currentText().isEmpty()){
		insertFild.append(", fild_nr");
		selectFild.append(", " + ui.comboBoxFieldNr->currentText());
	}
	if(ui.checkBoxGemark->isChecked() &&
			!ui.comboBoxGemark->currentText().isEmpty()){
		insertLot.append(", gmrk");
		selectLot.append(", " + ui.comboBoxGemark->currentText());
	}
	if(ui.checkBoxFlur->isChecked() &&
			!ui.comboBoxFlur->currentText().isEmpty()){
		insertLot.append(", flur");
		selectLot.append(", " + ui.comboBoxFlur->currentText());
	}
	if(ui.checkBoxLotNr->isChecked() &&
			!ui.comboBoxLotNr->currentText().isEmpty()){
		insertLot.append(", lot_nr");
		selectLot.append(", " + ui.comboBoxLotNr->currentText());
	}
	if(ui.checkBoxLotSubNr->isChecked() &&
			!ui.comboBoxLotSubNr->currentText().isEmpty()){
		insertLot.append(", lot_sub_nr");
		selectLot.append(", " + ui.comboBoxLotSubNr->currentText());
	}
	if(ui.checkBoxFieldLotSize->isChecked() &&
			!ui.comboBoxFieldLotSize->currentText().isEmpty()){
		insertLot.append(", lot_size");
		selectLot.append(", total(cast(replace("+ ui.comboBoxFieldLotSize->currentText() + ",',','.') AS REAL))");
	}


	QString fild_use_id;

	q.exec("SELECT min(fild_use_id) FROM fild_use");
	while(q.next()){
			fild_use_id = q.value(0).toString();
		}

	if(fild_use_id.isEmpty()){
		QMessageBox::information(this, "CuteFarm", "Die Tabelle 'Bodennutzungen' ist leer. Es muss mindestens ein "
				"Eintrag vorhanden sein.");
		return;
	}

	insertLot.remove(0, 1);
	insertLot.prepend("INSERT INTO lot (");
	insertLot.append(" , year_of_harvest) ");
	selectLot.remove(0, 1);
	selectLot.prepend("SELECT DISTINCT ");
	selectLot.append(" , year_of_harvest FROM csv GROUP BY "
			+ ui.comboBoxLotNr->currentText() + ", "
			+ ui.comboBoxLotSubNr->currentText() + ", "
			+ ui.comboBoxGemark->currentText());
	insertLot.append(selectLot);

	insertFild.remove(0, 1);
	insertFild.prepend("INSERT INTO fild (");
	insertFild.append(" , fild_use_id, year_of_harvest) ");
	selectFild.remove(0, 1);
	selectFild.prepend("SELECT DISTINCT ");
	selectFild.append(" , fild_use_id, year_of_harvest FROM csv "); //GROUP BY " + ui.comboBoxFieldNr->currentText());
	insertFild.append(selectFild);


	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	if(!q.exec(insertLot)){
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;
	}
	else if(q.lastError().type() != QSqlError::NoError){
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;
	}

	ui.progressBar->setValue(50);

	if(!q.exec(insertFild)){
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;

	}


	else if(q.lastError().type() != QSqlError::NoError){
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;
	}

	if (!q.exec("INSERT INTO lot_x_fild (lot_id, fild_id, lot_x_fild_size) "
				"SELECT lot_id, fild_id, total(cast(replace(" + ui.comboBoxFieldLotSize->currentText() + ", ',', '.') AS REAL)) AS size "
				"FROM lot, fild, csv "
				"WHERE (" + ui.comboBoxLotNr->currentText() + "=lot_nr) "
						"AND (" + ui.comboBoxLotSubNr->currentText() + "=lot_sub_nr) "
						"AND (" + ui.comboBoxFieldNr->currentText() + "=fild_nr) "
						"AND (" + ui.comboBoxGemark->currentText() + "=gmrk) "
						"AND ( user_id ="+userId+") "
						"GROUP BY  " +
						ui.comboBoxLotNr->currentText() + ", " +
						ui.comboBoxLotSubNr->currentText() + ", " +
						ui.comboBoxGemark->currentText() + ", " +
						ui.comboBoxFieldNr->currentText())){

		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;
	}

	else if(q.lastError().type() != QSqlError::NoError){
		QSqlDatabase::database().rollback();
		QMessageBox::critical(this, "Datenbankfehler", q.lastError().driverText() + "\n" +
				q.lastError().databaseText());
		qDebug() << q.lastQuery();
		ui.progressBar->hide();
		return;
	}
	qDebug() << q.lastQuery();

	qDebug() << "commit" << QSqlDatabase::database().commit();

	q.exec("INSERT INTO crop_use VALUES(NULL, NULL,'Grünland'");
	q.exec("INSERT INTO prod (fild_id, crop_use_id) SELECT f.fild_id , c.crop_use_id FROM "
			" (SELECT fild_id FROM fild WHERE user_id="+userId+" AND fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name='WIESEN')) AS f, "
					"(SELECT crop_use_id FROM crop_use WHERE crop_use_name='Grünland') AS c");



	ui.progressBar->setValue(100);
	this->accept();
}

void CsvImportDialog::check()
{
	QSqlQuery q;
	QList<QString> fildUses;



//	q.exec("SELECT DISTINCT KULTURART FROM csv WHERE KULTURART != ''");
//	while (q.next())
//		fildUses << q.value(0).toString().simplified();

	fildUses << "Acker" << "Wiesen" << "Weiden" << "Wald" << "Weg-Hoffläche";

	for (int i = 0; i < fildUses.size(); i++) {
		q.exec("INSERT INTO fild_use (fild_use_name) VALUES('"+fildUses.at(i)+"')");
	}

	QSqlDatabase::database().transaction();

	q.exec("UPDATE csv SET fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name='Wiesen') "
			" WHERE KULTURART='WIESEN'");

	q.exec("UPDATE csv SET fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name='Weiden') "
			" WHERE KULTURART='WEIDEN'");

	q.exec("UPDATE csv SET fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name='Wald') "
			" WHERE KULTURART='WALD'");

	q.exec("UPDATE csv SET fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name='Weg-Hoffläche') "
			" WHERE KULTURART='WEGHOFFL'");

	q.exec("UPDATE csv SET fild_use_id=(SELECT fild_use_id FROM fild_use WHERE fild_use_name ='Acker') "
			" WHERE KULTURART!='WIESEN'"
			" AND KULTURART !='WEIDEN'"
			" AND KULTURART !='WALD'"
			" AND KULTURART !='WEGHOFFL'");

	q.exec("UPDATE csv SET "+ui.comboBoxFieldName->currentText()+"='Schlag'"
			" WHERE "+ui.comboBoxFieldName->currentText()+"=''");

	q.exec("UPDATE csv SET "+ui.comboBoxFieldNr->currentText()+"=0"
			" WHERE "+ui.comboBoxFieldNr->currentText()+"=''");

	q.exec("UPDATE csv SET "+ui.comboBoxLotSubNr->currentText()+"=0"
			" WHERE "+ui.comboBoxLotSubNr->currentText()+"=''");

	q.exec("UPDATE csv SET "+ui.comboBoxFlur->currentText()+"=''"
				" WHERE "+ui.comboBoxFlur->currentText()+"=''");


	QSqlDatabase::database().commit();

	q.exec("SELECT lot_id, gmrk, lot_nr, lot_sub_nr, csv."+ui.comboBoxFieldNr->currentText()+" FROM csv, lot "
			" WHERE "+ui.comboBoxGemark->currentText()+"=lot.gmrk "
			" AND "+ui.comboBoxLotNr->currentText()+"=lot.lot_nr "
			" AND "+ui.comboBoxLotSubNr->currentText()+"=lot.lot_sub_nr ");

	qDebug() << q.lastError().text();

	QSqlQuery q2;
	QString info;

	while (q.next()) {
		qDebug() << q.value(0).toString() << q.value(1).toString() << q.value(2).toString()
					<< q.value(3).toString() << q.value(4).toString();

		info.append(q.value(1).toString()).append("\t");
		info.append(q.value(2).toString()).append("\t");
		info.append(q.value(3).toString()).append("\t");
		info.append(q.value(4).toString()).append("\n");

		q2.exec("DELETE FROM csv "
				" WHERE "+ui.comboBoxGemark->currentText()+"="+q.value(1).toString()+
				" AND "+ui.comboBoxLotNr->currentText()+"="+q.value(2).toString()+
				" AND "+ui.comboBoxLotSubNr->currentText()+"="+q.value(3).toString());
	}

	if (!info.isEmpty()) {
		QString header = "Folgende Flurstücke befinden sich bereits in der Datenbank und werden nicht importiert:\n\n";
		header.append(ui.comboBoxGemark->currentText()).append("\t");
		header.append(ui.comboBoxLotNr->currentText()).append("\t");
		header.append(ui.comboBoxLotSubNr->currentText()).append("\t");
		header.append(ui.comboBoxFieldNr->currentText()).append("\n\n");
		header.append(info);

		QTextEdit *te = new QTextEdit;
		te->setText(header);
		te->setAttribute(Qt::WA_DeleteOnClose);
		te->show();
		te->raise();
	}

}

CsvImportDialog::~CsvImportDialog()
{

}
