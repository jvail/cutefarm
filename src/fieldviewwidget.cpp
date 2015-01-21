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

#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QContextMenuEvent>
#include <QDebug>

#include "fieldviewwidget.h"
#include "settingsdialog.h"

FieldViewWidget::FieldViewWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	ui.groupBox->setTitle(tr("Nutrient Balance %1/%2")
	        .arg(SettingsDialog::nutUnit())
	        .arg(SettingsDialog::areaUnit()));

	QSqlDatabase::database().transaction();

	mFildModel = new QSqlQueryModel(this);
	mFildModel->setQuery(" SELECT DISTINCT fild_nr, fild_name "
			            " FROM fild ORDER BY fild_name ASC ");
	while (mFildModel->canFetchMore())
		mFildModel->fetchMore();
	ui.comboBoxFild->setModel(mFildModel);
	ui.comboBoxFild->setModelColumn(1);
	ui.comboBoxFild->setCurrentIndex(0);

	yearModel = new QSqlQueryModel(this);
	ui.comboBoxYear->setModel(yearModel);

	QSqlDatabase::database().commit();

	setupModels();

	if (!ui.comboBoxFild->currentText().isEmpty())
		onComboBoxFildCurrentIndexChanged(ui.comboBoxFild->currentIndex());

	connect(ui.comboBoxFild, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxFildCurrentIndexChanged(int)));

}

void FieldViewWidget::setupModels()
{
	drilActnModel = new SqlQueryModel(this);
	ui.tableViewDril->setModel(drilActnModel);

	harvActnModel = new SqlQueryModel(this);
	ui.tableViewHarv->setModel(harvActnModel);

	soilActnModel = new SqlQueryModel(this);
	ui.tableViewSoil->setModel(soilActnModel);

	medwActnModel = new SqlQueryModel(this);
	ui.tableViewMedw->setModel(medwActnModel);

	pltrActnModel = new SqlQueryModel(this);
	ui.tableViewPltr->setModel(pltrActnModel);

	fertActnModel = new SqlQueryModel(this);
	ui.tableViewFert->setModel(fertActnModel);

	animActnModel = new SqlQueryModel(this);
	ui.tableViewAnim->setModel(animActnModel);

	miscActnModel = new SqlQueryModel(this);
	ui.tableViewMisc->setModel(miscActnModel);

	soilSampleActnModel = new SqlQueryModel(this);
    ui.tableViewSoilTest->setModel(soilSampleActnModel);


    foreach (QTableView *view, findChildren<QTableView*>()) {
        view->horizontalHeader()->setHighlightSections(false);
        view->horizontalHeader()->setStretchLastSection(true);
        view->verticalHeader()->setHighlightSections(false);
        view->setContextMenuPolicy(Qt::CustomContextMenu);
        view->viewport()->installEventFilter(this);
    }
}

void FieldViewWidget::setModelHeaders()
{
    drilActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    drilActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    drilActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    drilActnModel->setHeaderData(4, Qt::Horizontal, tr("Crop Use (Cultivar)"));
    drilActnModel->setHeaderData(5, Qt::Horizontal, tr("Unit%1").arg(SettingsDialog::areaUnit()));
    drilActnModel->setHeaderData(6, Qt::Horizontal, tr("Sum"));
    drilActnModel->setHeaderData(7, Qt::Horizontal, tr("Comment"));

    harvActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    harvActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    harvActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    harvActnModel->setHeaderData(4, Qt::Horizontal, tr("Crop Use"));
    harvActnModel->setHeaderData(5, Qt::Horizontal, tr("Unit%1").arg("/" + SettingsDialog::areaUnit()));
    harvActnModel->setHeaderData(6, Qt::Horizontal, tr("Sum"));
    harvActnModel->setHeaderData(7, Qt::Horizontal, tr("Comment"));

    soilActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    soilActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    soilActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    soilActnModel->setHeaderData(5, Qt::Horizontal, tr("Comment"));

    medwActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    medwActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    medwActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    medwActnModel->setHeaderData(5, Qt::Horizontal, tr("Comment"));

    pltrActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    pltrActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    pltrActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    pltrActnModel->setHeaderData(4, Qt::Horizontal, tr("Input"));
    pltrActnModel->setHeaderData(5, Qt::Horizontal, tr("Unit%1").arg("/" + SettingsDialog::areaUnit()));
    pltrActnModel->setHeaderData(6, Qt::Horizontal, tr("Sum"));
    pltrActnModel->setHeaderData(7, Qt::Horizontal, tr("Comment"));

    fertActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    fertActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    fertActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    fertActnModel->setHeaderData(4, Qt::Horizontal, tr("Input"));
    fertActnModel->setHeaderData(5, Qt::Horizontal, tr("Unit%1").arg("/" + SettingsDialog::areaUnit()));
    fertActnModel->setHeaderData(6, Qt::Horizontal, tr("Sum"));
    fertActnModel->setHeaderData(7, Qt::Horizontal, tr("Comment"));

    animActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    animActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    animActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    animActnModel->setHeaderData(4, Qt::Horizontal, tr("Animal Group"));
    animActnModel->setHeaderData(5, Qt::Horizontal, tr("Number"));
    animActnModel->setHeaderData(6, Qt::Horizontal, tr("Comment"));

    miscActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    miscActnModel->setHeaderData(2, Qt::Horizontal, tr("Field"));
    miscActnModel->setHeaderData(3, Qt::Horizontal, tr("Area"));
    miscActnModel->setHeaderData(5, Qt::Horizontal, tr("Comment"));

    soilSampleActnModel->setHeaderData(1, Qt::Horizontal, tr("Date"));
    soilSampleActnModel->setHeaderData(2, Qt::Horizontal, "N");
    soilSampleActnModel->setHeaderData(3, Qt::Horizontal, "K2O");
    soilSampleActnModel->setHeaderData(4, Qt::Horizontal, "P2O5");
    soilSampleActnModel->setHeaderData(5, Qt::Horizontal, "S");
    soilSampleActnModel->setHeaderData(6, Qt::Horizontal, "MgO");
    soilSampleActnModel->setHeaderData(7, Qt::Horizontal, "CaO");
    soilSampleActnModel->setHeaderData(8, Qt::Horizontal, tr("Humus"));
    soilSampleActnModel->setHeaderData(9, Qt::Horizontal, "pH");
    soilSampleActnModel->setHeaderData(10, Qt::Horizontal, "Lon");
    soilSampleActnModel->setHeaderData(11, Qt::Horizontal, "Lat");
    soilSampleActnModel->setHeaderData(12, Qt::Horizontal, tr("Soil Type"));
    soilSampleActnModel->setHeaderData(13, Qt::Horizontal, tr("Other"));

    ui.tableViewDril->setColumnHidden(0, true);
    ui.tableViewHarv->setColumnHidden(0, true);
    ui.tableViewSoil->setColumnHidden(0, true);
    ui.tableViewSoil->setColumnHidden(4, true);
    ui.tableViewMedw->setColumnHidden(0, true);
    ui.tableViewMedw->setColumnHidden(4, true);
    ui.tableViewPltr->setColumnHidden(0, true);
    ui.tableViewFert->setColumnHidden(0, true);
    ui.tableViewAnim->setColumnHidden(0, true);
    ui.tableViewMisc->setColumnHidden(0, true);
    ui.tableViewMisc->setColumnHidden(4, true);
    ui.tableViewSoilTest->setColumnHidden(0, true);
    ui.tableViewDril->setColumnHidden(2, true);
    ui.tableViewHarv->setColumnHidden(2, true);
    ui.tableViewSoil->setColumnHidden(2, true);
    ui.tableViewSoil->setColumnHidden(2, true);
    ui.tableViewMedw->setColumnHidden(2, true);
    ui.tableViewMedw->setColumnHidden(2, true);
    ui.tableViewPltr->setColumnHidden(2, true);
    ui.tableViewFert->setColumnHidden(2, true);
    ui.tableViewAnim->setColumnHidden(2, true);
    ui.tableViewMisc->setColumnHidden(2, true);
    ui.tableViewMisc->setColumnHidden(2, true);
    ui.tableViewSoilTest->setColumnHidden(2, true);
}

void FieldViewWidget::updateFertBalance()
{
    int min = 0;
    int max = 0;
    QSqlQuery q;
    if (ui.cBFertBalanceYears->isChecked()) {
        q.prepare("SELECT total(n_kg_per_ha), total(k2o_kg_per_ha), total(p2o5_kg_per_ha),"
                " total(s_kg_per_ha), total(mgo_kg_per_ha), total(cao_kg_per_ha), total(humus_kg_per_ha) "
                " FROM v_nut_per_fild_id WHERE fild_nr=:nr GROUP BY fild_nr");
        q.bindValue(":nr", mFildModel->data(mFildModel->index(ui.comboBoxFild->currentIndex(), 0)));

//        q.prepare("SELECT n, k20, p2o5, s, mgo, cao, humus FROM v_fert_harv_balance_per_fild_nr WHERE fild_nr=:nr");
//        q.bindValue(":nr", mFildModel->data(mFildModel->index(ui.comboBoxFild->currentIndex(), 0)));
    }
    else {
        q.prepare("SELECT n_kg_per_ha, k2o_kg_per_ha, p2o5_kg_per_ha,"
                " s_kg_per_ha, mgo_kg_per_ha, cao_kg_per_ha, humus_kg_per_ha "
                " FROM v_nut_per_fild_id WHERE fild_id=:id");
        q.bindValue(":id", yearModel->data(yearModel->index(ui.comboBoxYear->currentIndex(), 0)));

//        q.prepare("SELECT n, k20, p2o5, s, mgo, cao, humus FROM v_fert_harv_balance_per_fild_id WHERE fild_id=:id");
//        q.bindValue(":id", yearModel->data(yearModel->index(ui.comboBoxYear->currentIndex(), 0)));
    }
    q.exec();
    if (q.first()) {
        for (int i = 0; i < q.record().count(); ++i) {
            min = qMin(min, q.value(i).toInt());
            max = qMax(max, q.value(i).toInt());
        }
        if (min > 0)
            min = 0;
        if (max < 0)
            max = 0;

        double basis = qAbs(min);
        double size = qAbs(min)+qAbs(max);


        ui.frameN->setValues(basis, size, qAbs(q.value(0).toInt()+basis));
        ui.labelN->setText(QString::number(q.value(0).toInt()));
        ui.frameK2O->setValues(basis, size, qAbs(q.value(1).toInt()+basis));
        ui.labelK2O->setText(QString::number(q.value(1).toInt()));
        ui.frameP2O5->setValues(basis, size, qAbs(q.value(2).toInt()+basis));
        ui.labelP2O5->setText(QString::number(q.value(2).toInt()));
        ui.frameS->setValues(basis, size, qAbs(q.value(3).toInt()+basis));
        ui.labelS->setText(QString::number(q.value(3).toInt()));
        ui.frameMgO->setValues(basis, size, qAbs(q.value(4).toInt()+basis));
        ui.labelMgO->setText(QString::number(q.value(4).toInt()));
        ui.frameCaO->setValues(basis, size, qAbs(q.value(5).toInt()+basis));
        ui.labelCaO->setText(QString::number(q.value(5).toInt()));
        ui.frameHumus->setValues(basis, size, qAbs(q.value(6).toInt()+basis));
        ui.labelHumus->setText(QString::number(q.value(6).toInt()));
    }
    else
        resetFertBalance();
}

void FieldViewWidget::resetFertBalance()
{
    ui.frameN->setValues(0, 0, 0);
    ui.labelN->setText("0");
    ui.frameK2O->setValues(0, 0, 0);
    ui.labelK2O->setText("0");
    ui.frameP2O5->setValues(0, 0, 0);
    ui.labelP2O5->setText("0");
    ui.frameS->setValues(0, 0, 0);
    ui.labelS->setText("0");
    ui.frameMgO->setValues(0, 0, 0);
    ui.labelMgO->setText("0");
    ui.frameCaO->setValues(0, 0, 0);
    ui.labelCaO->setText("0");
    ui.frameHumus->setValues(0, 0, 0);
    ui.labelHumus->setText("0");
}

void FieldViewWidget::refresh()
{
	mFildModel->setQuery(mFildModel->query().lastQuery());
	while (mFildModel->canFetchMore())
		mFildModel->fetchMore();
}

void FieldViewWidget::onDatabaseChanged()
{
    refresh();
}

void FieldViewWidget::onSettingsChanged()
{
    bool alternatingRowColors = SettingsDialog::alternatingRowColors();
    QHeaderView::ResizeMode resizeMode = SettingsDialog::resizeMode();
    bool showGrid = SettingsDialog::showGrid();

    foreach (QTableView *view, findChildren<QTableView*>()) {
        view->setAlternatingRowColors(alternatingRowColors);
        view->horizontalHeader()->setResizeMode(resizeMode);
        view->setShowGrid(showGrid);
    }
}

void FieldViewWidget::refreshTables()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    QSqlDatabase::database().transaction();
    drilActnModel->setQuery(drilActnModel->query().lastQuery());
    harvActnModel->setQuery(harvActnModel->query().lastQuery());
    soilActnModel->setQuery(soilActnModel->query().lastQuery());
    medwActnModel->setQuery(medwActnModel->query().lastQuery());
    pltrActnModel->setQuery(pltrActnModel->query().lastQuery());
    fertActnModel->setQuery(fertActnModel->query().lastQuery());
    animActnModel->setQuery(animActnModel->query().lastQuery());
    miscActnModel->setQuery(miscActnModel->query().lastQuery());
    soilSampleActnModel->setQuery(soilSampleActnModel->query().lastQuery());
    QSqlDatabase::database().commit();
    QApplication::restoreOverrideCursor();

//    setModelHeaders();
}

bool FieldViewWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::ContextMenu) {
        qDebug() << obj << ev;

        QTableView *view = qobject_cast<QTableView*>(obj->parent());
        if (view) {
            QContextMenuEvent *ce = static_cast<QContextMenuEvent*>(ev);
            emit actionTblContextMenuRequested(ce->globalPos());
        }
    }

    return QWidget::eventFilter(obj, ev);
}

void FieldViewWidget::on_cBFertBalanceYears_toggled(bool on)
{
    Q_UNUSED(on);
    updateFertBalance();
}

void FieldViewWidget::onComboBoxFildCurrentIndexChanged(int idx)
{
	QString qry =
	QString(" SELECT DISTINCT fild_id, year_of_harvest "
	        " FROM fild WHERE fild_nr=%1 "
            " ORDER BY year_of_harvest DESC")
            .arg(mFildModel->data(mFildModel->index(idx, 0)).toString());

	yearModel->setQuery(qry);
	while (yearModel->canFetchMore())
		yearModel->fetchMore();

	ui.comboBoxYear->setModelColumn(1);
}

void FieldViewWidget::on_comboBoxYear_currentIndexChanged(const QString &text)
{
	if (text.isEmpty()) {
	    resetFertBalance();
	    ui.lineEditCrop->clear();
	    ui.lineEditLot->clear();
	    ui.lineEditSize->clear();
	    ui.lineEditTyp->clear();
	    ui.labelEnvPrg->clear();
	    refreshTables();
		return;
	}

	int fildId = yearModel->data(yearModel->index(ui.comboBoxYear->currentIndex(), 0)).toInt();
	QLocale locale;

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	QSqlDatabase::database().transaction();

	QSqlQuery q;
	q.prepare(" SELECT crop_use_name "
			  " FROM crop_use, prod "
			  " WHERE prod.fild_id=:fildId "
			  " AND prod.crop_use_id=crop_use.crop_use_id ");
	q.bindValue(":fildId", fildId);
	q.exec();
	while (q.next())
	    ui.lineEditCrop->setText(q.value(0).toString());

	if (!q.first())
	    ui.lineEditCrop->setText(tr("No production added"));

	q.prepare(" SELECT fild_use_name "
			  " FROM fild, fild_use "
			  " WHERE fild.fild_id=:fildId "
			  " AND fild_use.fild_use_id=fild.fild_use_id ");
	q.bindValue(":fildId", fildId);
	q.exec();
	while (q.next())
	    ui.lineEditTyp->setText(q.value(0).toString());

	//env_prg
    q.prepare(" SELECT group_concat(evnt_prg_abbr, ' / ') "
              " FROM evnt_prg, fild_x_evnt_prg "
              " WHERE evnt_prg.evnt_prg_id=fild_x_evnt_prg.evnt_prg_id "
              " AND fild_x_evnt_prg.fild_id=:fildId "
              " GROUP BY fild_id");
    q.bindValue(":fildId", fildId);
    q.exec();
    if (q.first())
        ui.labelEnvPrg->setText(q.value(0).toString());
    else
        ui.labelEnvPrg->clear();


	q.prepare(" SELECT lots, total "
			  " FROM v_fild "
			  " WHERE v_fild.fild_id=:fildId ");
	q.bindValue(":fildId", fildId);
	q.exec();
	while (q.next()) {
	    ui.lineEditLot->setText(q.value(0).toString());
	    ui.lineEditSize->setText(locale.toString(q.value(1).toDouble(), 'f', 4).append(" " + SettingsDialog::areaUnit()));
	}

	drilActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, input_name, input_amt, sum_amt, fild_com "
			                " FROM actn, v_actn_dril "
			                " WHERE actn.actn_id=v_actn_dril.actn_id "
			                " AND v_actn_dril.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC " );
	drilActnModel->setDateCol(1);

	harvActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, input_name, input_amt, sum_amt, fild_com "
			                " FROM actn, v_actn_harv "
			                " WHERE actn.actn_id=v_actn_harv.actn_id "
			                " AND v_actn_harv.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC " );
	harvActnModel->setDateCol(1);

	soilActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, actn_soil_name, fild_com "
			                " FROM actn, v_actn_soil "
			                " WHERE actn.actn_id=v_actn_soil.actn_id "
			                " AND v_actn_soil.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC ");
	soilActnModel->setDateCol(1);

	medwActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, actn_medw_name, fild_com "
			                " FROM actn, v_actn_medw "
			                " WHERE actn.actn_id=v_actn_medw.actn_id "
			                " AND v_actn_medw.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC ");
	medwActnModel->setDateCol(1);

	pltrActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, input_name, input_amt, sum_amt, fild_com "
			                " FROM actn, v_actn_pltr "
			                " WHERE actn.actn_id=v_actn_pltr.actn_id "
			                " AND v_actn_pltr.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC ");
	pltrActnModel->setDateCol(1);

	fertActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, input_name, input_amt, sum_amt, fild_com "
			                " FROM actn, v_actn_fert "
			                " WHERE actn.actn_id=v_actn_fert.actn_id "
			                " AND v_actn_fert.fild_id=" + QString::number(fildId)  +
				            " ORDER BY actn_date DESC ");
	fertActnModel->setDateCol(1);

	animActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, anim_grp_name, input_amt, fild_com "
			                " FROM actn, v_actn_anim_grp "
			                " WHERE actn.actn_id=v_actn_anim_grp.actn_id "
			                " AND v_actn_anim_grp.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC ");
	animActnModel->setDateCol(1);

	miscActnModel->setQuery(" SELECT actn.actn_id, date(actn_date) AS actn_date, fild_name, area, actn_misc_name, fild_com "
			                " FROM actn, v_actn_misc "
			                " WHERE actn.actn_id=v_actn_misc.actn_id "
			                " AND v_actn_misc.fild_id=" + QString::number(fildId) +
				            " ORDER BY actn_date DESC ");
	miscActnModel->setDateCol(1);

	soilSampleActnModel->setQuery(" SELECT "
	        "actn.actn_id,"
	        "date(actn_date),"
            "soil_test_n,"
            "soil_test_k2o,"
            "soil_test_p2o5,"
            "soil_test_s,"
            "soil_test_mgo,"
            "soil_test_cao,"
            "soil_test_humus,"
            "soil_test_ph,"
            "soil_test_lon,"
            "soil_test_lat,"
            "soil_test_soil_type,"
            "soil_test_other "
            " FROM actn_soil_test, actn, actn_fild "
            " WHERE actn_soil_test.actn_id=actn.actn_id "
            " AND actn.actn_id=actn_fild.actn_id "
            " AND actn_fild.fild_id="  + QString::number(fildId) +
            " ORDER BY actn_date DESC ");
	soilSampleActnModel->setDateCol(1);

	updateFertBalance();

	QSqlDatabase::database().commit();

	setModelHeaders();
	QApplication::restoreOverrideCursor();
}

FieldViewWidget::~FieldViewWidget()
{

}
