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

#include <QtGui>
#include <QSqlError>

#include "actionwizard.h"
//#include "../3rdparty/wwwidgets/qwwrichtextedit.h"

//TODO: standarditemmodel is evil. redesign everything

ActionWizard::ActionWizard(QWidget *parent, int actnId, Mode mode, const QDate &date)
    : QWizard(parent, (Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint))
{
//#ifdef Q_WS_WIN
//	setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
//#endif
//	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//#ifndef Q_WS_HILDON
//	QVariant w = height() * 1.33;
//	resize(w.toInt(), height());
//#else
//	resize(maximumWidth(), maximumHeight());
//#endif
	setSizeGripEnabled(true);
	setDefaultProperty("QDoubleSpinBox", "value", "valueChanged");

	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));

	m_mode = mode;
	m_actnId = 1;
	m_year = -1;

	qDebug() << "ActionWizard transaction" << QSqlDatabase::database().transaction();

//	QSqlQuery qActnId("SELECT max(actn_id) FROM actn");
//	while (qActnId.next())
//		m_actnId = qActnId.value(0).toInt() + 1;

	if (actnId > -1) {
		QSqlQuery qYear("SELECT year_of_harvest FROM actn WHERE actn_id=" + QString::number(actnId));
			while (qYear.next())
				m_year = qYear.value(0).toInt();
	}

	fildModel = new QStandardItemModel(0, 5, this);
	pltrModel = new QStandardItemModel(0, 6, this);
	fertModel = new QStandardItemModel(0, 5, this);
	drilModel = new QStandardItemModel(0, 4, this);
	harvModel = new QStandardItemModel(0, 4, this);
	animModel = new QStandardItemModel(0, 3, this);
	proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(fildModel);
	proxyModel->setFilterKeyColumn(1);
	proxyModel->setDynamicSortFilter(true);
	proxyModel->setFilterRole(Qt::CheckStateRole);
	proxyModel->setFilterFixedString(QString::number(Qt::Checked));

	QSqlQuery q;
	q.exec("SELECT id, name, substance, type, pltr_unit || '/" + SettingsDialog::areaUnit() + "' "
			"FROM v_pltr");
    int row = 0;
	while(q.next()){
		for(int col = 0; col < 6; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 5){
	    		item->setData(0.00 , Qt::DisplayRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    	}
	    	else{
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    	}
	    	pltrModel->setItem(row, col, item);
	   }
        row++;
    }

	q.exec("SELECT fert_id, fert_name, fert_typ_name, fert_unit || '/" + SettingsDialog::areaUnit() + "' "
			"FROM fert, fert_typ "
			"WHERE fert.fert_typ_id=fert_typ.fert_typ_id "
			"ORDER BY fert_name ASC");
    row = 0;
	while(q.next()){
		for(int col = 0; col < 5; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 4){
	    		item->setData(0.00 , Qt::DisplayRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    	}
	    	else{
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    	}
	    	fertModel->setItem(row, col, item);
	   }
        row++;
    }

	q.exec("SELECT seed_id, seed_name, seed_unit || '/" + SettingsDialog::areaUnit() + "' "
			"FROM v_seedV2 "
			"ORDER BY seed_name ASC");
    row = 0;
	while(q.next()){
		for(int col = 0; col < 4; col++){
	    	QStandardItem *drilItem = new QStandardItem;
//	    	QStandardItem *harvItem = new QStandardItem;
	    	if(col == 2){
	    		drilItem->setData(0.00 , Qt::DisplayRole);
	    		drilItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
//	    		harvItem->setData(0.00 , Qt::DisplayRole);
//	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	    	}
	    	else if(col == 1){
	    		drilItem->setData(q.value(col), Qt::DisplayRole);
	    		drilItem->setData(q.value(col), Qt::ToolTipRole);
	    		drilItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		drilItem->setCheckState(Qt::Unchecked);
//	    		harvItem->setData(q.value(col), Qt::DisplayRole);
//	    		harvItem->setData(q.value(col), Qt::ToolTipRole);
//	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
//	    		harvItem->setCheckState(Qt::Unchecked);
	    	}
            else if (col == 3) {
                drilItem->setData(q.value(2), Qt::DisplayRole);
                drilItem->setData(q.value(2), Qt::ToolTipRole);
                drilItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            }
	    	else{
	    		drilItem->setData(q.value(col), Qt::DisplayRole);
	    		drilItem->setData(q.value(col), Qt::ToolTipRole);
	    		drilItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//	    		harvItem->setData(q.value(col), Qt::DisplayRole);
//	    		harvItem->setData(q.value(col), Qt::ToolTipRole);
//	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    	}
	    	drilModel->setItem(row, col, drilItem);
//	    	harvModel->setItem(row, col, harvItem);
	   }
        row++;
    }

	q.exec("SELECT crop_use_id, crop_use_name, crop_use_harv_unit || '/" + SettingsDialog::areaUnit() + "' "
			" FROM crop_use "
			" WHERE crop_use_is_actv='true' OR crop_use_id IN (SELECT DISTINCT crop_use_id FROM prod) "
			" ORDER BY crop_use_name ASC");
    row = 0;
	while(q.next()){
		for(int col = 0; col < 4; col++){
	    	QStandardItem *harvItem = new QStandardItem;
	    	if(col == 2){
	    		harvItem->setData(0.00 , Qt::DisplayRole);
	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	    	}
	    	else if(col == 1){
	    		harvItem->setData(q.value(col), Qt::DisplayRole);
	    		harvItem->setData(q.value(col), Qt::ToolTipRole);
	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		harvItem->setCheckState(Qt::Unchecked);
	    	}
	    	else if (col == 0) {
	    		harvItem->setData(q.value(col), Qt::DisplayRole);
	    		harvItem->setData(q.value(col), Qt::ToolTipRole);
	    		harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    	}
            else if (col == 3) {
                harvItem->setData(q.value(2), Qt::DisplayRole);
                harvItem->setData(q.value(2), Qt::ToolTipRole);
                harvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            }
	    	harvModel->setItem(row, col, harvItem);
	   }
        row++;
    }

	q.exec("SELECT anim_grp_id, anim_grp_name, anim_grp_cont "
			"FROM anim_grp "
			"WHERE anim_grp_is_actv='true'"
			"ORDER BY anim_spc_id ASC");
    row = 0;
	while(q.next()){
		for(int col = 0; col < 3; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 2){
	    		item->setData(q.value(col).toInt() , Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setData(q.value(col).toInt(), Qt::UserRole+1);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    	}
	    	else{
	    		item->setData(q.value(col), Qt::DisplayRole);
	    		item->setData(q.value(col), Qt::ToolTipRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    	}
	    	animModel->setItem(row, col, item);
	   }
        row++;
    }

	qDebug() << "ActionWizard commit" << QSqlDatabase::database().commit();

//	qDebug() << "1";

	setPage(Page_Action, new ActionWizardActnPage(date, this));
//	 qDebug() << "2";
	setPage(Page_Fild, new ActionWizardFildPage(this, fildModel, m_year));
//	 qDebug() << "3";
    setPage(Page_Comment, new ActionWizardComtPage(this, proxyModel));
//    qDebug() << "4";
    setPage(Page_Animal, new ActionWizardAnimPage(this, animModel));
//    qDebug() << "5";
    setPage(Page_Drill, new ActionWizardDrilPage(this, drilModel));
//    qDebug() << "6";
    setPage(Page_Harvest, new ActionWizardHarvPage(this, harvModel));
//    qDebug() << "7";
    setPage(Page_Fertiliser, new ActionWizardFertPage(this, fertModel));
//    qDebug() << "8";
    setPage(Page_Planttreatment, new ActionWizardPltrPage(this, pltrModel));

    setPage(Page_SoilTesting, new ActionWizardSoilTestPage(this));

        setStartId(Page_Action);

//        qDebug() << "9";

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif

#ifndef Q_WS_HILDON
    setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/action.png"));
#endif

    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    connect(this, SIGNAL(accepted()), this, SLOT(finish()));

    setOptions(QWizard::NoBackButtonOnStartPage | QWizard::HaveHelpButton);
    setWindowTitle(tr("Add Action"));
    setWindowIcon(QIcon(":/pic/add_action.png"));
    setDefaultProperty("QComboBox", "currentText", SIGNAL(currentIndexChanged()));
    setDefaultProperty("QDateEdit", "date", SIGNAL(dateChanged()));

	if (m_mode != New && actnId > -1 && m_year > -1) {
		m_actnId = actnId;
		if (m_mode == Update) {
			qDebug() << "Update: m_actnId" << m_actnId;
			setWindowIcon(QIcon(":/pic/edit_action.png"));
			setWindowTitle(tr("Edit Action"));
		}
		loadActnData();
	}
	if (m_year < 0)
	    m_mode = New;
	if (m_mode != Update) {
		qDebug() << "Copy: m_actnId" << m_actnId;
		QSqlQuery qActnId("SELECT max(actn_id) FROM actn");
		while (qActnId.next())
			m_actnId = qActnId.value(0).toInt() + 1;
		qDebug() << "Copy: m_actnId" << m_actnId;
	}

}

void ActionWizard::loadActnData()
{
	qDebug() << "ActionWizard::loadActnData() transaction" << QSqlDatabase::database().transaction();

	QSqlQuery q;
	ActionWizardActnPage *actnPage = qobject_cast<ActionWizardActnPage *>(page(ActionWizard::Page_Action));
	ActionWizardComtPage *comtPage = qobject_cast<ActionWizardComtPage *>(page(ActionWizard::Page_Comment));

	q.prepare(" SELECT work_id, oprt_id, actn_date, actn_com "
			  " FROM actn "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		actnPage->setWorkerId(q.value(0).toInt());
		actnPage->setOprtId(q.value(1).toInt());
		actnPage->setDate(q.value(2).toString());
		comtPage->setActnComt(q.value(3).toString());
	}


	q.prepare(" SELECT fild_id, area, fild_com "
			  " FROM actn_fild "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < fildModel->rowCount(); i++) {
			if (fildModel->data(fildModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				fildModel->setData(fildModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				fildModel->setData(fildModel->index(i, 2), q.value(1).toDouble(), Qt::DisplayRole);
				fildModel->setData(fildModel->index(i, 4), q.value(2), Qt::DisplayRole);
			}
		}
	}

	if (!q.first())
		actnPage->setMiscChecked(true);

	q.prepare(" SELECT pltr_id, actn_pltr_amt "
			  " FROM actn_pltr "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < pltrModel->rowCount(); i++) {
			if (pltrModel->data(pltrModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				pltrModel->setData(pltrModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				pltrModel->setData(pltrModel->index(i, 5), q.value(1).toDouble(), Qt::DisplayRole);
			}
		}
	}

	if (q.first())
		actnPage->setPltrChecked(true);

	q.prepare(" SELECT fert_id, actn_fert_amt "
			  " FROM actn_fert "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < fertModel->rowCount(); i++) {
			if (fertModel->data(fertModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				fertModel->setData(fertModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				fertModel->setData(fertModel->index(i, 4), q.value(1).toDouble(), Qt::DisplayRole);
			}
		}
	}

	if (q.first())
		actnPage->setFertChecked(true);

	q.prepare(" SELECT seed_id, actn_dril_amt "
			  " FROM actn_dril "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < drilModel->rowCount(); i++) {
			if (drilModel->data(drilModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				drilModel->setData(drilModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				drilModel->setData(drilModel->index(i, 2), q.value(1).toDouble(), Qt::DisplayRole);
			}
		}
	}

	if (q.first())
		actnPage->setDrilChecked(true);

	q.prepare(" SELECT crop_use_id, actn_harv_amt "
			  " FROM actn_harv "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < harvModel->rowCount(); i++) {
			if (harvModel->data(harvModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				harvModel->setData(harvModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				harvModel->setData(harvModel->index(i, 2), q.value(1).toDouble(), Qt::DisplayRole);
			}
		}
	}

	if (q.first())
		actnPage->setHarvChecked(true);

	q.prepare(" SELECT anim_grp_id, actn_anim_grp_amt "
			  " FROM actn_anim_grp "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	while (q.next()) {
		for (int i = 0; i < animModel->rowCount(); i++) {
			if (animModel->data(animModel->index(i, 0), Qt::DisplayRole).toInt() == q.value(0).toInt()) {
				animModel->setData(animModel->index(i, 1), Qt::Checked, Qt::CheckStateRole);
				animModel->setData(animModel->index(i, 2), q.value(1).toDouble(), Qt::DisplayRole);
			}
		}
	}

	if (q.first())
		actnPage->setAnimChecked(true);

	q.prepare(" SELECT actn_soil_id "
			  " FROM actn_soil "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	if (q.first())
		actnPage->setSoilChecked(true);

	q.prepare(" SELECT actn_medw_id "
			  " FROM actn_medw "
			  " WHERE actn_id=:actnId ");
	q.bindValue(":actnId", m_actnId);
	q.exec();

	if (q.first())
		actnPage->setMedwChecked(true);

	ActionWizardSoilTestPage *soilTestPage = qobject_cast<ActionWizardSoilTestPage *>(page(ActionWizard::Page_SoilTesting));
    q.prepare(" SELECT "
            "soil_test_n,"
            "soil_test_k2o,"
            "soil_test_p2o5,"
            "soil_test_mgo,"
            "soil_test_cao,"
            "soil_test_s,"
            "soil_test_humus,"
            "soil_test_ph,"
            "soil_test_lon,"
            "soil_test_lat,"
            "soil_test_soil_type,"
            "soil_test_other "
            " FROM actn_soil_test "
            " WHERE actn_id=:actnId ");
    q.bindValue(":actnId", m_actnId);
    q.exec();
    while (q.next()) {
        soilTestPage->ui.dsbN->setValue(q.value(0).toDouble());
        soilTestPage->ui.dsbK2O->setValue(q.value(1).toDouble());
        soilTestPage->ui.dsbP2O5->setValue(q.value(2).toDouble());
        soilTestPage->ui.dsbMgO->setValue(q.value(3).toDouble());
        soilTestPage->ui.dsbCaO->setValue(q.value(4).toDouble());
        soilTestPage->ui.dsbS->setValue(q.value(5).toDouble());
        soilTestPage->ui.dsbHumus->setValue(q.value(6).toDouble());
        soilTestPage->ui.dsbPh->setValue(q.value(7).toDouble());
        soilTestPage->ui.dsbLon->setValue(q.value(8).toDouble());
        soilTestPage->ui.dsbLat->setValue(q.value(9).toDouble());
        soilTestPage->ui.leSoilType->setText(q.value(10).toString());
        soilTestPage->ui.leOther->setText(q.value(11).toString());
    }

    if (q.first())
        actnPage->setSoilTestChecked(true);

	qDebug() << "ActionWizard::loadActnData() commit" << QSqlDatabase::database().commit();
}

QList<QString> ActionWizard::acntList() const
{
	return m_actnList;
}

QMultiMap<QString, QString> ActionWizard::acntMap() const
{
	return m_actnMap;
}

ActionWizard::Mode ActionWizard::currentMode() const
{
	return m_mode;
}

void ActionWizard::finish()
{
	// Prüfung: Wurde 'Speichern' schon gedrückt
	if( !testOption( QWizard::HaveCustomButton1 ) && m_mode != Update )
		return;

//	m_actnList.clear();
	m_actnMap.clear();

	//test
	QList<int> oprtList = list(ActionWizard::List_Oprt);
	QString str;
	for(int i = 0; i < oprtList.size(); i++){
		str.append(QString::number(oprtList.at(i)) + ", ");
	}

	QList<int> fildList = list(ActionWizard::List_Fild);
	str.clear();
	for(int i = 0; i < fildList.size(); i++){
		str.append(QString::number(fildList.at(i)) + ", ");
	}

	QList<int> pagesToVisitList = list(ActionWizard::List_PagesToVisit);
	str.clear();
	for(int i = 0; i < pagesToVisitList.size(); i++){
		str.append(QString::number(pagesToVisitList.at(i)) + ", ");
	}

	// Prüfung: sind alle List_PagesToVisit auch in visitedPages
	for( int i = 0; i < pagesToVisitList.size(); ++i ) {
		if( !visitedPages().contains( pagesToVisitList.at( i ) ) )
			QMessageBox::information( 0, "Nicht Besucht??", QString::number( pagesToVisitList.at( i ) ) );
	}

	ActionWizardActnPage *actnPage = qobject_cast<ActionWizardActnPage *>(page(ActionWizard::Page_Action));
	ActionWizardFildPage *fildPage = qobject_cast<ActionWizardFildPage *>(page(ActionWizard::Page_Fild));
	ActionWizardComtPage *comtPage = qobject_cast<ActionWizardComtPage *>(page(ActionWizard::Page_Comment));

	QString actn_id = QString::number(m_actnId);
	QString actnDate = actnPage->date();
	int workId  = actnPage->workerId();
	QString work_name = actnPage->workName();
	int oprtId = actnPage->oprtId();
	QString oprt_name = actnPage->oprtName();
	int year_of_harvest = fildPage->year();
	QString actnComt = comtPage->actnComment();

//	m_actnList << QString::number(m_actnId) << actnDate << work_name << oprt_name << QString::number(year_of_harvest) << actnComt;
	m_actnMap.insert("id", QString::number(m_actnId));
	m_actnMap.insert("date", actnDate);
	m_actnMap.insert("work", work_name);
	m_actnMap.insert("oprt", oprt_name);
	m_actnMap.insert("year", QString::number(year_of_harvest));
	m_actnMap.insert("comt", actnComt);



	QSqlQuery q;
	qDebug() << "transaction" << QSqlDatabase::database().transaction();

	if (m_mode == Update) {

		q.prepare(" DELETE FROM actn_fild "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_pltr "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_fert "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_soil "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_medw "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_dril "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_harv "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_anim_grp "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

		q.prepare(" DELETE FROM actn_misc "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}

        q.prepare(" DELETE FROM actn_soil_test "
                  " WHERE actn_id=:actnId ");
        q.bindValue(":actnId", m_actnId);
        q.exec();

        if ( q.lastError().type() != QSqlError::NoError ) {
            qDebug() << "DELETE rollback" << QSqlDatabase::database().rollback();
            QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
            return;
        }

		q.prepare(" UPDATE actn "
				  " SET actn_date=:actnDate, work_id=:workId, oprt_id=:oprtId, year_of_harvest=:year, actn_com=:actnCom "
				  " WHERE actn_id=:actnId ");
		q.bindValue(":actnDate", actnDate);
		q.bindValue(":workId", workId);
		q.bindValue(":oprtId", oprtId);
		q.bindValue(":year", year_of_harvest);
		q.bindValue(":actnCom", actnComt);
		q.bindValue(":actnId", m_actnId);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "UPDATE rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}
	}
	else {

		q.prepare(" INSERT INTO actn (actn_id, actn_date, work_id, oprt_id, year_of_harvest, actn_com) "
				  " VALUES (:actnId, :actnDate, :workId, :oprtId, :year, :actnCom) ");
		q.bindValue(":actnId", m_actnId);
		q.bindValue(":actnDate", actnDate);
		q.bindValue(":workId", workId);
		q.bindValue(":oprtId", oprtId);
		q.bindValue(":year", year_of_harvest);
		q.bindValue(":actnCom", actnComt);
		q.exec();

		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}
	}

	QVariant fildId;
	QVariant area;
	QString fildCom;

    for (int row = 0; row < fildModel->rowCount(); ++row) {
        if (fildModel->data(fildModel->index(row, 1, QModelIndex()), Qt::CheckStateRole).toInt() != Qt::Checked)
            continue;

        fildId = fildModel->data(fildModel->index(row, 0, QModelIndex()), Qt::DisplayRole);
        area = fildModel->data(fildModel->index(row, 2, QModelIndex()), Qt::DisplayRole);
        fildCom = fildModel->data(fildModel->index(row, 4, QModelIndex()), Qt::DisplayRole).toString().trimmed();
        q.prepare(" INSERT INTO actn_fild (actn_fild_id, actn_id, fild_id, area, fild_com) "
                  " VALUES (NULL, :actnDate, :fildId, :area, :fildCom) ");
        q.bindValue(":actnId", m_actnId);
        q.bindValue(":fildId", fildId);
        q.bindValue(":area", area);
        q.bindValue(":fildCom", fildCom);
        q.exec();
        if ( q.lastError().type() != QSqlError::NoError ) {
            qDebug() << "rollback" << QSqlDatabase::database().rollback();
            QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
            return;
        }
    }

	if (oprtList.contains(ActionWizard::Oprt_Soil)) {

		QString actnSoilName = tr("Tillage");

		m_actnMap.insert("use", actnSoilName);

		q.prepare(" INSERT INTO actn_soil (actn_soil_id, actn_id, actn_soil_name) "
				  " VALUES (NULL, :actnId, :actnSoilName) ");
		q.bindValue(":actnId", m_actnId);
		q.bindValue(":actnSoilName", actnSoilName);
		q.exec();
		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Medw)) {

		QString actnMedwName = tr("Grassland Action");
		m_actnMap.insert("use", actnMedwName);

		q.prepare(" INSERT INTO actn_medw (actn_medw_id, actn_id, actn_medw_name) "
				  " VALUES (NULL, :actnId, :actnMedwName) ");
		q.bindValue(":actnId", m_actnId);
		q.bindValue(":actnMedwName", actnMedwName);
		q.exec();
		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Pltr)) {

		int pltrId;
		double actnPltrAmt;
		m_actnMap.insert("use", tr("Plant Treatment"));

		for (int row = 0; row < pltrModel->rowCount(); row++) {
			if (pltrModel->data(pltrModel->index(row, 1), Qt::CheckStateRole) == Qt::Checked) {
				pltrId = pltrModel->data(pltrModel->index(row, 0, QModelIndex()), Qt::DisplayRole).toInt();
				actnPltrAmt = pltrModel->data(pltrModel->index(row, 5, QModelIndex()), Qt::DisplayRole).toDouble();
				q.prepare(" INSERT INTO actn_pltr (actn_pltr_id, actn_id, pltr_id, actn_pltr_amt) "
						  " VALUES (NULL, :actnId, :pltrId, :actnPltrAmt) ");
				q.bindValue(":actnId", m_actnId);
				q.bindValue(":pltrId", pltrId);
				q.bindValue(":actnPltrAmt", actnPltrAmt);
				q.exec();
				if ( q.lastError().type() != QSqlError::NoError ) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
					return;
				}
			}
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Fert)) {

		int fertId;
		double actnFertAmt;
		m_actnMap.insert("use", tr("Fetilisation"));

		for (int row = 0; row < fertModel->rowCount(); row++) {
			if (fertModel->data(fertModel->index(row, 1), Qt::CheckStateRole) == Qt::Checked) {
				fertId = fertModel->data(fertModel->index(row, 0), Qt::DisplayRole).toInt();
				actnFertAmt = fertModel->data(fertModel->index(row, 4), Qt::DisplayRole).toDouble();
				q.prepare(" INSERT INTO actn_fert (actn_fert_id, actn_id, fert_id, actn_fert_amt) "
						  " VALUES (NULL, :actnId, :fertId, :actnFertAmt) ");
				q.bindValue(":actnId", m_actnId);
				q.bindValue(":fertId", fertId);
				q.bindValue(":actnFertAmt", actnFertAmt);
				q.exec();
				if ( q.lastError().type() != QSqlError::NoError ) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
					return;
				}
			}
		}
	}


	if (oprtList.contains(ActionWizard::Oprt_Dril)) {

		int seedId;
		double actnSeedAmt;
		m_actnMap.insert("use", tr("Drilling&Planting"));

		for (int row = 0; row < drilModel->rowCount(); row++) {
			if (drilModel->data(drilModel->index(row, 1), Qt::CheckStateRole) == Qt::Checked) {
				seedId = drilModel->data(drilModel->index(row, 0), Qt::DisplayRole).toInt();
				actnSeedAmt = drilModel->data(drilModel->index(row, 2), Qt::DisplayRole).toDouble();
				q.prepare(" INSERT INTO actn_dril (actn_dril_id, actn_id, seed_id, actn_dril_amt) "
						  " VALUES (NULL, :actnId, :seedId, :actnSeedAmt) ");
				q.bindValue(":actnId", m_actnId);
				q.bindValue(":seedId", seedId);
				q.bindValue(":actnSeedAmt", actnSeedAmt);
				q.exec();
				if ( q.lastError().type() != QSqlError::NoError ) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
					return;
				}
			}
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Harv)) {

		int cropUseId;
		double actnHarvAmt;
		m_actnMap.insert("use", tr("Harvest"));

		for (int row = 0; row < harvModel->rowCount(); row++) {
			if (harvModel->data(harvModel->index(row, 1), Qt::CheckStateRole) == Qt::Checked) {
				cropUseId = harvModel->data(harvModel->index(row, 0), Qt::DisplayRole).toInt();
				actnHarvAmt = harvModel->data(harvModel->index(row, 2), Qt::DisplayRole).toDouble();
				q.prepare(" INSERT INTO actn_harv (actn_harv_id, actn_id, crop_use_id, actn_harv_amt) "
						  " VALUES (NULL, :actnId, :cropUseId, :actnSeedAmt) ");
				q.bindValue(":actnId", m_actnId);
				q.bindValue(":cropUseId", cropUseId);
				q.bindValue(":actnSeedAmt", actnHarvAmt);
				q.exec();
				if ( q.lastError().type() != QSqlError::NoError ) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
					return;
				}
			}
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Anim)) {

		int animGrpId;
		int animGrpAmt;
		m_actnMap.insert("use", tr("Animal Action"));

		for (int row = 0; row < animModel->rowCount(); row++) {
			if (animModel->data(animModel->index(row, 1), Qt::CheckStateRole) == Qt::Checked) {
				animGrpId = animModel->data(animModel->index(row, 0), Qt::DisplayRole).toInt();
				animGrpAmt = animModel->data(animModel->index(row, 2), Qt::DisplayRole).toInt();
				q.prepare(" INSERT INTO actn_anim_grp (actn_anim_grp_id, actn_id, anim_grp_id, actn_anim_grp_amt) "
						  " VALUES (NULL, :actnId, :animGrpId, :animGrpAmt) ");
				q.bindValue(":actnId", m_actnId);
				q.bindValue(":animGrpId", animGrpId);
				q.bindValue(":animGrpAmt", animGrpAmt);
				q.exec();
				if ( q.lastError().type() != QSqlError::NoError ) {
					qDebug() << "rollback" << QSqlDatabase::database().rollback();
					QMessageBox::information( this, tr("Database Error"), q.lastError().text() + "\n" + q.lastQuery());
					return;
				}
			}
		}
	}

	if (oprtList.contains(ActionWizard::Oprt_Misc)) {

		QString actnMiscName = tr("Misc. Action");
		m_actnMap.insert("use", actnMiscName);

		q.prepare(" INSERT INTO actn_misc (actn_misc_id, actn_id, actn_misc_name) "
				  " VALUES (NULL, :actnId, :actnMiscName) ");
		q.bindValue(":actnId", m_actnId);
		q.bindValue(":actnMiscName", actnMiscName);
		q.exec();
		if ( q.lastError().type() != QSqlError::NoError ) {
			qDebug() << "rollback" << QSqlDatabase::database().rollback();
			QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
			return;
		}
	}

    if (oprtList.contains(ActionWizard::Oprt_SoilTesting)) {

        QString actnSoilTestName = tr("Soil Sample");

        q.prepare(" INSERT INTO actn_soil_test (actn_soil_test_id, actn_id,"
                "soil_test_n,"
                "soil_test_k2o,"
                "soil_test_p2o5,"
                "soil_test_mgo,"
                "soil_test_cao,"
                "soil_test_s,"
                "soil_test_humus,"
                "soil_test_ph,"
                "soil_test_lon,"
                "soil_test_lat,"
                "soil_test_soil_type,"
                "soil_test_other,"
                "actn_soil_test_name) "
                " VALUES (NULL, :actnId,"
                ":soil_test_n,"
                ":soil_test_k2o,"
                ":soil_test_p2o5,"
                ":soil_test_mgo,"
                ":soil_test_cao,"
                ":soil_test_s,"
                ":soil_test_humus,"
                ":soil_test_ph,"
                ":soil_test_lon,"
                ":soil_test_lat,"
                ":soil_test_soil_type,"
                ":soil_test_other,"
                ":actn_soil_test_name)");
        q.bindValue(":actnId", m_actnId);
        q.bindValue(":soil_test_n", field("n"));
        q.bindValue(":soil_test_k2o", field("k2o"));
        q.bindValue(":soil_test_p2o5", field("p2o5"));
        q.bindValue(":soil_test_mgo", field("mgo"));
        q.bindValue(":soil_test_cao", field("cao"));
        q.bindValue(":soil_test_s", field("s"));
        q.bindValue(":soil_test_humus", field("humus"));
        q.bindValue(":soil_test_ph", field("ph"));
        q.bindValue(":soil_test_lon", field("lon"));
        q.bindValue(":soil_test_lat", field("lat"));
        q.bindValue(":soil_test_soil_type", field("type"));
        q.bindValue(":soil_test_other", field("other"));
        q.bindValue(":actn_soil_test_name", actnSoilTestName);
        q.exec();
        if ( q.lastError().type() != QSqlError::NoError ) {
            qDebug() << "rollback" << QSqlDatabase::database().rollback();
            QMessageBox::information( this, tr("Database Error"), q.lastError().text() );
            return;
        }
    }




	qDebug() << "commit" << QSqlDatabase::database().commit();

	if (m_mode == Update) {  //reset to Add Action
	    setWindowTitle(tr("Add Action"));
	    setWindowIcon(QIcon(":/pic/add_action.png"));
	    m_mode = New;
	}
//	else {
//		emit signalAcntMap(m_actnMap);
//	}

	q.exec("SELECT max(actn_id) FROM actn");
	while(q.next()){
			m_actnId = q.value(0).toInt() + 1;
	}

	setOption(QWizard::HaveCustomButton1, false);
}

QList<int> ActionWizard::list(int ListType) const
{
	if(ListType == ActionWizard::List_Fild){
		QList<int> fildlist;
		for(int row = 0; row < fildModel->rowCount(QModelIndex()); row++){
			if(fildModel->data(fildModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
				fildlist << fildModel->data(fildModel->index(row, 0, QModelIndex()), Qt::DisplayRole).toInt();
			}
		}
		return fildlist;
	}

	if(ListType == ActionWizard::List_PagesToVisit){
		ActionWizardActnPage *actnPage = qobject_cast<ActionWizardActnPage *>(page(ActionWizard::Page_Action));
		QList<int> pagesToVisitList = actnPage->dataList(ActionWizard::List_PagesToVisit);
		return pagesToVisitList;
	}

	if(ListType == ActionWizard::List_Oprt){
		ActionWizardActnPage *actnPage = qobject_cast<ActionWizardActnPage *>(page(ActionWizard::Page_Action));
		QList<int> oprtList = actnPage->dataList(ActionWizard::List_Oprt);
		return oprtList;
	}

	else
		return QList<int>();
}

void ActionWizard::showHelp()
{
    QString message;

    switch (currentId()) {
    case Page_Action:
        message = tr("The presented wizard pages "
        		"depend on selected action types.");
        break;
    case Page_Fild:
        message = tr("If you like to add an action, which does not depend "
        		"on a specific field (soil) e.g. a machine repair do not select "
        		"anything else but 'Misc. Action' on the previous page.\n"
        		"If not the area of each selected field must be > 0.00.");
        break;
    case Page_Comment:
        message = tr("Add Comments - Action and/or field specific.");
        break;
    case Page_Animal:
        message = tr("At least one animal group must be selected and the "
        		"number of animals in each group must be > 0.");
        break;
    case Page_Drill:
        message = tr("At least one seed must be selected and each seed amount "
        		"must be > 0. If you are sowing mixtures you can create them in "
        		"the 'General Data' Dialog.");
        break;
    case Page_Harvest:
        message = tr("At least one crop usage must be selected and each harvested "
        		"amount must be > 0.\n"
        		"The database does not care if you have sown any of the crops harvested "
        		"on a specific field or if you have tagged the field (production) with "
        		"the one you are harvesting here.");
        break;
    case Page_Fertiliser:
        message = tr("Select at least one fertilizer. Each selected fertilizer's amount must "
        		"be > 0.");
        break;
    case Page_Planttreatment:
        message = tr("Select at least one plant treatment with an amount > 0.");
//        message = tr("Es muß mindestens ein Mittel ausgewählt sein und die "
//         		"Aufwandmenge aller ausgewählen Mittel muß > 0 sein.");
        break;
    case Page_SoilTesting:
        message = tr("Add your lab's result.");
        break;
    default:
        message = tr("No help available.");
    }

    QMessageBox::information(this, "CuteFarm", message);
}

void ActionWizard::onCurrentIdChanged(int id)
{
    if (id == Page_Fild) {
        for (int row = 0; row < fildModel->rowCount(); ++row)
            fildModel->item(row, 1)->setEnabled(true);
    }
}



ActionWizardActnPage::ActionWizardActnPage(const QDate &date, QWidget *parent)
    : QWizardPage(parent)
{

	ui.setupUi(this);

	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/action.png").scaledToHeight(40, Qt::SmoothTransformation));

	registerField("comboBoxOprt", ui.comboBoxOprt, "currentText", SIGNAL(currentIndexChanged()));
	registerField("comboBoxWorker", ui.comboBoxWorker, "currentText", SIGNAL(currentIndexChanged()));
	registerField("dateEdit", ui.dateEdit, "date", SIGNAL(dateChanged()));



    connect(ui.comboBoxOprt, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCompleteChanged()));
    connect(ui.comboBoxWorker, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCompleteChanged()));

	ui.dateEdit->setDate(date);

    oprtModel = new QSqlQueryModel(this);
    oprtModel->setQuery("SELECT DISTINCT oprt_id, oprt_name FROM oprt WHERE oprt_is_actv='true' "
    		"ORDER BY oprt_name ASC");
    while(oprtModel->canFetchMore())
    	oprtModel->fetchMore();
    ui.comboBoxOprt->setModel(oprtModel);
    ui.comboBoxOprt->setModelColumn(1);
    ui.comboBoxOprt->setCurrentIndex(0);

    workModel = new QSqlQueryModel(this);
    workModel->setQuery("SELECT DISTINCT work_id, work_name FROM work WHERE work_is_actv='true' "
    		"ORDER BY work_name ASC");
    while(workModel->canFetchMore())
    	workModel->fetchMore();
    ui.comboBoxWorker->setModel(workModel);
    ui.comboBoxWorker->setModelColumn(1);
    ui.comboBoxWorker->setCurrentIndex(0);


    connect(ui.checkBoxSoil, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxMeadow, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxPltr, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxFert, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxDrill, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxHarv, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxAnim, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxMisc, SIGNAL(stateChanged(int)),
    		this, SLOT(emitCompleteChanged()));
    connect(ui.checkBoxSoilTesting, SIGNAL(stateChanged(int)),
            this, SLOT(emitCompleteChanged()));

}

int ActionWizardActnPage::workerId()
{
	return workModel->data(workModel->index(ui.comboBoxWorker->currentIndex(), 0, QModelIndex())).toInt();
}

void ActionWizardActnPage::setWorkerId(int id)
{
	QList<QModelIndex> idx = workModel->match(workModel->index(0, 0), Qt::DisplayRole, id, 1);
	qDebug() << "ActionWizardActnPage::setWorkerId idx" << idx;
	if (!idx.isEmpty())
		ui.comboBoxWorker->setCurrentIndex(idx.first().row());
}

QString ActionWizardActnPage::workName()
{
	return workModel->data(workModel->index(ui.comboBoxWorker->currentIndex(), 1, QModelIndex())).toString();
}

QString ActionWizardActnPage::date()
{
	return ui.dateEdit->date().toString("yyyy-MM-dd");
}

void ActionWizardActnPage::setDate(QString date)
{
	ui.dateEdit->setDate(QDate::fromString(date, "yyyy-MM-dd"));
}

int ActionWizardActnPage::oprtId()
{
	return oprtModel->data(oprtModel->index(ui.comboBoxOprt->currentIndex(), 0, QModelIndex())).toInt();
}

void ActionWizardActnPage::setOprtId(int id)
{
	QList<QModelIndex> idx = oprtModel->match(oprtModel->index(0, 0), Qt::DisplayRole, id, 1);
	qDebug() << "ActionWizardActnPage::setOprtId idx" << idx;
	if (!idx.isEmpty())
		ui.comboBoxOprt->setCurrentIndex(idx.first().row());
}

QString ActionWizardActnPage::oprtName()
{
	return oprtModel->data(oprtModel->index(ui.comboBoxOprt->currentIndex(), 1, QModelIndex())).toString();
}

void ActionWizardActnPage::setMiscChecked(bool checked)
{
	ui.checkBoxMisc->setChecked(checked);
}

void ActionWizardActnPage::setPltrChecked(bool checked)
{
	ui.checkBoxPltr->setChecked(checked);
}

void ActionWizardActnPage::setFertChecked(bool checked)
{
	ui.checkBoxFert->setChecked(checked);
}

void ActionWizardActnPage::setDrilChecked(bool checked)
{
	ui.checkBoxDrill->setChecked(checked);
}

void ActionWizardActnPage::setHarvChecked(bool checked)
{
	ui.checkBoxHarv->setChecked(checked);
}

void ActionWizardActnPage::setSoilChecked(bool checked)
{
	ui.checkBoxSoil->setChecked(checked);
}

void ActionWizardActnPage::setMedwChecked(bool checked)
{
	ui.checkBoxMeadow->setChecked(checked);
}

void ActionWizardActnPage::setAnimChecked(bool checked)
{
	ui.checkBoxAnim->setChecked(checked);
}

void ActionWizardActnPage::setSoilTestChecked(bool checked)
{
    ui.checkBoxSoilTesting->setChecked(checked);
}

void ActionWizardActnPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardActnPage::isComplete() const
{
	if(!ui.comboBoxOprt->currentText().isEmpty() &&
			!ui.comboBoxWorker->currentText().isEmpty() &&
			!dataList(ActionWizard::List_Oprt).isEmpty()){
		return true;
	}
	else
		return false;
}

int ActionWizardActnPage::nextId() const
{

	QList<int> pagesToVisit = dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty()){
		return ActionWizard::Page_Action;
	}

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();

}

QList<int> ActionWizardActnPage::dataList(int listType) const
{

	if(listType == ActionWizard::List_Oprt){
		QList<int> oprt_islist;
		if(ui.checkBoxSoil->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Soil;
		}
		if(ui.checkBoxMeadow->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Medw;
		}
		if(ui.checkBoxPltr->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Pltr;
		}
		if(ui.checkBoxFert->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Fert;
		}
		if(ui.checkBoxDrill->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Dril;
		}
		if(ui.checkBoxHarv->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Harv;
		}
		if(ui.checkBoxAnim->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Anim;
		}
		if(ui.checkBoxMisc->checkState() == Qt::Checked){
			oprt_islist << ActionWizard::Oprt_Misc;
		}
        if(ui.checkBoxSoilTesting->checkState() == Qt::Checked){
            oprt_islist << ActionWizard::Oprt_SoilTesting;
        }

		return oprt_islist;
	}
	else if(listType == ActionWizard::List_PagesToVisit){
		QList<int> pagesToVisit;
		pagesToVisit << ActionWizard::Page_Fild;
		if(ui.checkBoxPltr->checkState() == Qt::Checked){
			pagesToVisit << ActionWizard::Page_Planttreatment;
		}
		if(ui.checkBoxFert->checkState() == Qt::Checked){
			pagesToVisit << ActionWizard::Page_Fertiliser;
		}
		if(ui.checkBoxDrill->checkState() == Qt::Checked){
			pagesToVisit << ActionWizard::Page_Drill;
		}
		if(ui.checkBoxHarv->checkState() == Qt::Checked){
			pagesToVisit << ActionWizard::Page_Harvest;
		}
		if(ui.checkBoxAnim->checkState() == Qt::Checked){
			pagesToVisit << ActionWizard::Page_Animal;
		}
        if(ui.checkBoxSoilTesting->checkState() == Qt::Checked){
            pagesToVisit << ActionWizard::Page_SoilTesting;
        }
//		if(ui.checkBoxSoil->checkState() == Qt::Checked ||
//				ui.checkBoxMeadow->checkState() == Qt::Checked ||
//				ui.checkBoxMisc->checkState() == Qt::Checked){
//			pagesToVisit << ActionWizard::Page_Comment;
//		}
		pagesToVisit << ActionWizard::Page_Comment;
		return pagesToVisit;
	}
	else
		return QList<int>();
}



ActionWizardFildPage::ActionWizardFildPage(QWidget *parent, QStandardItemModel *fildModel, int year)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/field.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
	m_fildModel = fildModel;
	m_fildModel->setHeaderData(1, Qt::Horizontal, tr("Field"));
	m_fildModel->setHeaderData(2, Qt::Horizontal, tr("Area", "the actual area worked on during a field operation."));
	m_fildModel->setHeaderData(3, Qt::Horizontal, tr("Production"));
	m_fildModel->setHeaderData(4, Qt::Horizontal, tr("Comment"));
    ui.tableViewFild->setModel(m_fildModel);
    ui.tableViewFild->setItemDelegateForColumn(2, new SpinBoxDelegate(ui.tableViewFild, 0, 0, true, true, SettingsDialog::areaUnit(), 4));
	ui.tableViewFild->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewFild->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewFild->horizontalHeader()->setStretchLastSection(true);
	ui.tableViewFild->resizeRowsToContents();
    ui.tableViewFild->hideColumn(0);
    ui.tableViewFild->hideColumn(4);

    yearModel = new QSqlQueryModel;
    yearModel->setQuery("SELECT DISTINCT year_of_harvest FROM fild ORDER BY year_of_harvest DESC");
    while(yearModel->canFetchMore())
    	yearModel->fetchMore();
    ui.comboBoxYear->setModel(yearModel);

    if (year > -1) {
    	QList<QModelIndex> idx = yearModel->match(yearModel->index(0, 0), Qt::DisplayRole, year, 1);
    	if (!idx.isEmpty())
    		ui.comboBoxYear->setCurrentIndex(idx.first().row());
    }

    connect(ui.comboBoxYear, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCompleteChanged()));
    connect(m_fildModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(selectAll(int)));


}

int ActionWizardFildPage::year()
{
	return ui.comboBoxYear->currentText().toInt();
}

void ActionWizardFildPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardFildPage::isComplete() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> oprtList = page->dataList(ActionWizard::List_Oprt);
	if(oprtList.size() == 1){
		if(oprtList.first() == ActionWizard::Oprt_Misc){
			return true;
		}
	}

	bool b = false;
	for(int row = 0; row < m_fildModel->rowCount(QModelIndex()); row++){
		if(m_fildModel->data(m_fildModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_fildModel->data(m_fildModel->index(row, 2, QModelIndex()), Qt::DisplayRole).toDouble() == 0.00){
				return false;
			}
			else
				b = true;
			}
		}
	return b;
}

void ActionWizardFildPage::selectAll(int state)
{
	if(state == Qt::Checked){
		for(int row = 0; row < m_fildModel->rowCount(QModelIndex()); row++){
				m_fildModel->item(row, 1)->setData(Qt::Checked, Qt::CheckStateRole);
				}
			}
	else {
		for(int row = 0; row < m_fildModel->rowCount(QModelIndex()); row++){
				m_fildModel->item(row, 1)->setData(Qt::Unchecked, Qt::CheckStateRole);
				}
			}
}

void ActionWizardFildPage::on_comboBoxYear_currentIndexChanged(const QString &year)
{
	if(!year.isEmpty()){
		QSqlQuery qFild(" SELECT prod.fild_id, fild_name, total, crop_use_name "
				        " FROM prod, v_fild, crop_use "
				        " WHERE prod.fild_id=v_fild.fild_id "
				        " AND prod.crop_use_id=crop_use.crop_use_id "
				        " AND v_fild.year=" + year);
		m_fildModel->removeRows(0, m_fildModel->rowCount(QModelIndex()), QModelIndex());
	    int row = 0;
		while(qFild.next()){
			for(int col = 0; col < 5; col++){
		    	QStandardItem *item = new QStandardItem;
		    	if(col == 2){
		    		item->setData(qFild.value(col).toDouble(), Qt::DisplayRole);
		    		item->setData(locale.toString(qFild.value(col).toDouble()), Qt::ToolTipRole);
		    		item->setData(qFild.value(col).toDouble(), Qt::UserRole+1);
		    	}
		    	else if(col == 1){
		    		item->setData(qFild.value(col), Qt::DisplayRole);
		    		item->setData(qFild.value(col), Qt::ToolTipRole);
		    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
//		    		item->setCheckable(true);
		    		item->setCheckState(ui.checkBox->checkState());
		    	}
		    	else if(col == 4){
		    		item->setData("", Qt::DisplayRole);
		    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		    	}
		    	else{
		    		item->setData(qFild.value(col), Qt::DisplayRole);
		    		item->setData(qFild.value(col), Qt::ToolTipRole);
		    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		    	}

		    	m_fildModel->setItem(row, col, item);

//		    	if(col ==1)
//		        QMessageBox::information(0,"fild", item->data(Qt::CheckStateRole).toString());
			}

	        row++;
	    }
//		ui.tableViewField->resizeRowsToContents();
	}
//	ui.tableViewField->resizeRowsToContents();

//	emitCompleteChanged();
//	if(year.isEmpty())
//		return;
//	else
//		prodModel->setFilter("prod.fild_id IN (SELECT fild_id FROM fild WHERE year_of_harvest=" + year + ")");
}

void ActionWizardFildPage::initializePage()
{
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ", "'at' a certain date") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ", "'by' a certain worker") + field("comboBoxWorker").toString());
}

int ActionWizardFildPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty())
		return ActionWizard::Page_Action;

	for(int row = 0; row < m_fildModel->rowCount(QModelIndex()); row++){
		if(m_fildModel->data(m_fildModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){

			if(wizard()->visitedPages().size()-1 == pagesToVisit.size())
				return -1;

			for(int i = 0; i < pagesToVisit.size(); ++i){
				if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
							return 	pagesToVisit.at(i);
			         }
		}
	}

	if(pagesToVisit.contains(ActionWizard::Page_Animal))
		return 	ActionWizard::Page_Animal;


	return ActionWizard::Page_Comment;
}



ActionWizardAnimPage::ActionWizardAnimPage(QWidget *parent, QStandardItemModel *animModel)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/cow.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
    m_animModel = animModel;
    m_animModel->setHeaderData(1, Qt::Horizontal, tr("Animal Group"));
    m_animModel->setHeaderData(2, Qt::Horizontal, tr("Number of Animals"));
    ui.tableViewAnim->setModel(m_animModel);
    ui.tableViewAnim->setItemDelegateForColumn(2, new SpinBoxDelegate(ui.tableViewAnim,0 , 0, false, true, tr(" Number", "Number [unit] of animals")));
	ui.tableViewAnim->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewAnim->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewAnim->horizontalHeader()->setStretchLastSection(true);
    ui.tableViewAnim->hideColumn(0);

    connect(m_animModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));

}

void ActionWizardAnimPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardAnimPage::isComplete() const
{
	bool b = false;
	for(int row = 0; row < m_animModel->rowCount(QModelIndex()); row++){
		if(m_animModel->data(m_animModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_animModel->data(m_animModel->index(row, 2, QModelIndex()), Qt::DisplayRole).toInt() == 0){
				return false;
			}
			else
				b = true;
		}
	}
	return b;
}

void ActionWizardAnimPage::initializePage()
{
#ifndef Q_WS_HILDON
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ", "'on' a certain day. Leave spaces!") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ", "'by' a certain worker. Leave spaces!") + field("comboBoxWorker").toString());
#else
	setSubTitle(QString());
#endif
}

int ActionWizardAnimPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty())
		return ActionWizard::Page_Action;

	if(wizard()->visitedPages().size()-1 == pagesToVisit.size())
		return -1;

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();
}



ActionWizardDrilPage::ActionWizardDrilPage(QWidget *parent, QStandardItemModel *drilModel)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/crop.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
    m_drilModel = drilModel;
    m_drilModel->setHeaderData(1, Qt::Horizontal, tr("Seed (Cultivar)"));
    m_drilModel->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    m_drilModel->setHeaderData(3, Qt::Horizontal, tr("Unit"));
    ui.tableViewCrop->setModel(m_drilModel);
    ui.tableViewCrop->setItemDelegateForColumn(2, new SpinBoxDelegate(ui.tableViewCrop,0 , 999, true, false, tr(" unit%1").arg("/" + SettingsDialog::areaUnit())));
	ui.tableViewCrop->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewCrop->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewCrop->horizontalHeader()->setStretchLastSection(true);
	ui.tableViewCrop->resizeColumnsToContents();
    ui.tableViewCrop->hideColumn(0);

    connect(m_drilModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));

}

void ActionWizardDrilPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardDrilPage::isComplete() const
{
	bool b = false;
	for(int row = 0; row < m_drilModel->rowCount(QModelIndex()); row++){
		if(m_drilModel->data(m_drilModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_drilModel->data(m_drilModel->index(row, 2, QModelIndex()), Qt::DisplayRole).toDouble() == 0.00){
				return false;
			}
			else
				b = true;
		}
	}
	return b;
}

void ActionWizardDrilPage::initializePage()
{
#ifndef Q_WS_HILDON
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ") + field("comboBoxWorker").toString());
#else
	setSubTitle(QString());
#endif
}

int ActionWizardDrilPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty())
		return ActionWizard::Page_Action;

	if(wizard()->visitedPages().size()-1 == pagesToVisit.size())
		return -1;

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();
}



ActionWizardFertPage::ActionWizardFertPage(QWidget *parent, QStandardItemModel *fertModel)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/fert.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
    m_fertModel = fertModel;
    m_fertModel->setHeaderData(1, Qt::Horizontal, tr("Fertiliser"));
    m_fertModel->setHeaderData(2, Qt::Horizontal, tr("Type", "..of fertiliser"));
    m_fertModel->setHeaderData(3, Qt::Horizontal, tr("Unit"));
    m_fertModel->setHeaderData(4, Qt::Horizontal, tr("Amount"));
    ui.tableViewFert->setModel(m_fertModel);
    ui.tableViewFert->setItemDelegateForColumn(4, new SpinBoxDelegate(ui.tableViewFert,0 , 999, true, false, tr(" Unit/%1").arg(SettingsDialog::areaUnit())));
	ui.tableViewFert->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewFert->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewFert->horizontalHeader()->setStretchLastSection(true);
    ui.tableViewFert->hideColumn(0);

    connect(m_fertModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));

}

void ActionWizardFertPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardFertPage::isComplete() const
{
	bool b = false;
	for(int row = 0; row < m_fertModel->rowCount(QModelIndex()); row++){
		if(m_fertModel->data(m_fertModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_fertModel->data(m_fertModel->index(row, 4, QModelIndex()), Qt::DisplayRole).toDouble() == 0.00){
				return false;
			}
			else
				b = true;
		}
	}
	return b;
}

void ActionWizardFertPage::initializePage()
{
#ifndef Q_WS_HILDON
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ") + field("comboBoxWorker").toString());
#else
	setSubTitle(QString());
#endif
}

int ActionWizardFertPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty())
		return ActionWizard::Page_Action;

	if(wizard()->visitedPages().size()-1 == pagesToVisit.size())
		return -1;

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();
}



ActionWizardHarvPage::ActionWizardHarvPage(QWidget *parent, QStandardItemModel *harvModel)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/cut.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
    m_harvModel = harvModel;
    m_harvModel->setHeaderData(1, Qt::Horizontal, tr("Crop Use"));
    m_harvModel->setHeaderData(2, Qt::Horizontal, tr("Amount", "..of harvest"));
    m_harvModel->setHeaderData(3, Qt::Horizontal, tr("Unit"));
    ui.tableViewCrop->setModel(m_harvModel);
    ui.tableViewCrop->setItemDelegateForColumn(2, new SpinBoxDelegate(ui.tableViewCrop,0 , 999, true, false, tr(" Unit/%1").arg(SettingsDialog::areaUnit())));
	ui.tableViewCrop->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewCrop->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewCrop->horizontalHeader()->setStretchLastSection(true);
	ui.tableViewCrop->resizeColumnsToContents();
    ui.tableViewCrop->hideColumn(0);

    connect(m_harvModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));

}

void ActionWizardHarvPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardHarvPage::isComplete() const
{
	bool b = false;
	for(int row = 0; row < m_harvModel->rowCount(QModelIndex()); row++){
		if(m_harvModel->data(m_harvModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_harvModel->data(m_harvModel->index(row, 2, QModelIndex()), Qt::DisplayRole).toDouble() == 0.00){
				return false;
			}
			else
				b = true;
		}
	}
	return b;
}

void ActionWizardHarvPage::initializePage()
{
#ifndef Q_WS_HILDON
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ") + field("comboBoxWorker").toString());
#else
	setSubTitle(QString());
#endif
}

int ActionWizardHarvPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty())
		return ActionWizard::Page_Action;

	if(wizard()->visitedPages().size()-1 == pagesToVisit.size())
		return -1;

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();
}



ActionWizardPltrPage::ActionWizardPltrPage(QWidget *parent,
		QStandardItemModel *pltrModel)
    : QWizardPage(parent)
{
    ui.setupUi(this);
#ifndef Q_WS_HILDON
	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/pltr.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif
    m_pltrModel = pltrModel;
    m_pltrModel->setHeaderData(1, Qt::Horizontal, tr("Plant Treatment"));
    m_pltrModel->setHeaderData(2, Qt::Horizontal, tr("Active Ingredients"));
    m_pltrModel->setHeaderData(3, Qt::Horizontal, tr("Type"));
    m_pltrModel->setHeaderData(4, Qt::Horizontal, tr("Unit"));
    m_pltrModel->setHeaderData(5, Qt::Horizontal, tr("Amount"));
    ui.tableViewPltr->setModel(m_pltrModel);
    ui.tableViewPltr->setItemDelegateForColumn(5, new SpinBoxDelegate(ui.tableViewPltr,0 , 999, true, false, tr(" Unit/%1").arg(SettingsDialog::areaUnit())));
	ui.tableViewPltr->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewPltr->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
	ui.tableViewPltr->horizontalHeader()->setStretchLastSection(true);
    ui.tableViewPltr->hideColumn(0);

    connect(m_pltrModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(emitCompleteChanged()));

}

void ActionWizardPltrPage::emitCompleteChanged()
{
	emit completeChanged();
}

bool ActionWizardPltrPage::isComplete() const
{
	bool b = false;
	for(int row = 0; row < m_pltrModel->rowCount(QModelIndex()); row++){
		if(m_pltrModel->data(m_pltrModel->index(row, 1, QModelIndex()), Qt::CheckStateRole) == Qt::Checked){
			if(m_pltrModel->data(m_pltrModel->index(row, 5, QModelIndex()), Qt::DisplayRole).toDouble() == 0.00){
				return false;
			}
			else
				b = true;
		}
	}
	return b;
}

void ActionWizardPltrPage::initializePage()
{
#ifndef Q_WS_HILDON
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ") + field("comboBoxWorker").toString());
	ui.tableViewPltr->horizontalHeader()->setResizeMode(SettingsDialog::resizeMode());
#else
	setSubTitle(QString());
#endif
}

int ActionWizardPltrPage::nextId() const
{
	ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
	QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
	if(pagesToVisit.isEmpty()){
		return ActionWizard::Page_Action;
	}

	if(wizard()->visitedPages().size()-1 == pagesToVisit.size()){
		return -1;
	}

	for(int i = 0; i < pagesToVisit.size(); ++i){
		if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
					return 	pagesToVisit.at(i);
	         }

	return wizard()->currentId();
}


ActionWizardSoilTestPage::ActionWizardSoilTestPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Soil Sample"));
    ui.setupUi(this);
#ifndef Q_WS_HILDON
    setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/soiltesting32.png").scaledToHeight(40, Qt::SmoothTransformation));
#endif

    registerField("cao", ui.dsbCaO);
    registerField("humus", ui.dsbHumus);
    registerField("k2o", ui.dsbK2O);
    registerField("lat", ui.dsbLat);
    registerField("lon", ui.dsbLon);
    registerField("mgo", ui.dsbMgO);
    registerField("n", ui.dsbN);
    registerField("p2o5", ui.dsbP2O5);
    registerField("s", ui.dsbS);
    registerField("ph", ui.dsbPh);
    registerField("type", ui.leSoilType);
    registerField("other", ui.leOther);

}

bool ActionWizardSoilTestPage::isComplete() const
{
    return true;
}

void ActionWizardSoilTestPage::initializePage()
{
#ifndef Q_WS_HILDON
    setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
            + tr(" by ") + field("comboBoxWorker").toString());

#else
    setSubTitle(QString());
#endif
}

int ActionWizardSoilTestPage::nextId() const
{
    ActionWizardActnPage *page = qobject_cast<ActionWizardActnPage *>(wizard()->page(ActionWizard::Page_Action));
    QList<int> pagesToVisit = page->dataList(ActionWizard::List_PagesToVisit);
    if(pagesToVisit.isEmpty()){
        return ActionWizard::Page_Action;
    }

    if(wizard()->visitedPages().size()-1 == pagesToVisit.size()){
        return -1;
    }

    for(int i = 0; i < pagesToVisit.size(); ++i){
        if(!wizard()->visitedPages().contains(pagesToVisit.at(i)))
                    return  pagesToVisit.at(i);
             }

    return wizard()->currentId();
}



ActionWizardComtPage::ActionWizardComtPage(QWidget *parent, QSortFilterProxyModel *proxy)
    : QWizardPage(parent)
{
    ui.setupUi(this);

    mProxy = proxy;

	setPixmap(QWizard::LogoPixmap, QPixmap(":/pic/comment.png").scaledToHeight(40, Qt::SmoothTransformation));

	ui.tableViewFild->setVisible(false);
	ui.tableViewFild->setModel(proxy);
	ui.tableViewFild->setAlternatingRowColors(SettingsDialog::alternatingRowColors());
	ui.tableViewFild->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui.tableViewFild->horizontalHeader()->setStretchLastSection(true);
    ui.tableViewFild->hideColumn(0);
    ui.tableViewFild->hideColumn(2);
    ui.tableViewFild->hideColumn(3);

    connect(ui.checkBoxFildComt, SIGNAL(stateChanged(int)), this, SLOT(toggleFildCom(int)));

}

QString ActionWizardComtPage::actnComment()
{
	if (ui.qwwrichtextedit->toPlainText().isEmpty())
		return "";

	return ui.qwwrichtextedit->toHtml();
}

void ActionWizardComtPage::setActnComt(QString comt)
{
	ui.qwwrichtextedit->insertHtml(comt);
}

void ActionWizardComtPage::toggleFildCom(int state)
{
	if(state == Qt::Checked)
	{
		ui.tableViewFild->showColumn(4);

	}
	else
		ui.tableViewFild->hideColumn(4);
}

void ActionWizardComtPage::initializePage()
{
	setSubTitle("'" + field("comboBoxOprt").toString() + "'" + tr(" on ") + field("dateEdit").toDate().toString("dddd d MMMM yyyy")
			+ tr(" by ") + field("comboBoxWorker").toString());

	//TODO: schrottig
	QStandardItemModel *model = qobject_cast<QStandardItemModel*>(mProxy->sourceModel());

	for (int row = 0; row < model->rowCount(); ++row) {
	    model->item(row, 1)->setEnabled(false);
    }
}

int ActionWizardComtPage::nextId() const
{
	return -1;
}

void ActionWizardComtPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

    ActionWizard *w = qobject_cast<ActionWizard *>(wizard());

    if (visible && w->currentMode() != ActionWizard::Update) {
        wizard()->setButtonText(QWizard::CustomButton1, tr("Save"));
        wizard()->setOption(QWizard::HaveCustomButton1, true);
        connect(wizard(), SIGNAL(customButtonClicked(int)),
        		wizard(), SLOT(finish()));
    } else {
        wizard()->setOption(QWizard::HaveCustomButton1, false);
        disconnect(wizard(), SIGNAL(customButtonClicked(int)),
        		wizard(), SLOT(finish()));
    }
}


