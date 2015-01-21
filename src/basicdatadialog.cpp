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

#include <QSqlQuery>
#include <QSqlDriver>

#include "basicdatadialog.h"
#include "cropdialog.h"
#include "cropusedialog.h"
#include "cultivardialog.h"
#include "textinputdialog.h"
#include "pltrdialog.h"
#include "fertdialog.h"
#include "evntprgdialog.h"
#include "spinboxdelegate.h"

BasicDataDialog::BasicDataDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint)
{
	ui.setupUi(this);

	ui.toolButtonIsActive->setVisible(false);

	QVariant w = height() * 1.33;
	resize(w.toInt(), height());

	useMenu = new QMenu(this);
	useMenu->addAction(ui.actionAddUse);

	cropMenu = new QMenu(this);
	cropMenu->addAction(ui.actionAddCrop);
	cropMenu->addAction(ui.actionAddCropUse);
	cropMenu->addAction(ui.actionAddCultivar);

	pltrMenu = new QMenu(this);
	pltrMenu->addAction(ui.actionAddPltr);
	pltrMenu->addAction(ui.actionEditPltr);
	pltrMenu->addSeparator();
	pltrMenu->addAction(ui.actionAddPltrTyp);
	pltrMenu->addAction(ui.actionAddPltrSub);

	fertMenu = new QMenu(this);
	fertMenu->addAction(ui.actionAddFert);

	animMenu = new QMenu(this);
	animMenu->addAction(ui.actionAddAnim);

	ecoMenu = new QMenu(this);
	ecoMenu->addAction(ui.actionAddEvntPrg);


	setupModels();
	changeCurrentIndex(true);


    connect(ui.buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(buttonClose()));
    connect(ui.buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()),
            this, SLOT(buttonSave()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()),
            this, SLOT(buttonReset()));

	ui.buttonBox->button(QDialogButtonBox::Save)->hide();
	ui.buttonBox->button(QDialogButtonBox::Save)->setDefault(true);
	ui.buttonBox->button(QDialogButtonBox::Reset)->hide();

}

void BasicDataDialog::setupModels()
{
	QSqlDatabase::database().transaction();

	useModel = new SqlTableModel(this);
	useModel->setTable("fild_use");
	useModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	useModel->setHeaderData(1, Qt::Horizontal, tr("Field Use", "pasture, arable land etc."));
	useModel->addTextColumn(1);
	useModel->setBoolColumn(2);
	useModel->setSort(2, Qt::DescendingOrder);
	useModel->select();

	ui.tableViewUse->setModel(useModel);
	ui.tableViewUse->hideColumn(0);
	ui.tableViewUse->hideColumn(2);

	connect(useModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(useModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	cropModel = new SqlTableModel(this);
	cropModel->setTable("crop");
	cropModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	cropModel->setHeaderData(1, Qt::Horizontal, tr("Crop Abbreviation", "Unique crop code"));
	cropModel->setHeaderData(2, Qt::Horizontal, tr("Crop Name", "common name"));
	cropModel->setHeaderData(3, Qt::Horizontal, tr("lat. Crop Name", "latin name"));
	cropModel->addTextColumn(1);
	cropModel->addTextColumn(2);
	cropModel->addTextColumn(3);
	cropModel->select();
	ui.tableViewCrop->setModel(cropModel);
	ui.tableViewCrop->hideColumn(0);

	connect(cropModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(cropModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	cropUseModel = new SqlRelationalTableModel(this);
	cropUseModel->setTable("crop_use");
	cropUseModel->setRelation(1, QSqlRelation("crop", "crop_id", "crop_name"));
	cropUseModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	cropUseModel->setHeaderData(1, Qt::Horizontal, tr("Crop Name"));
	cropUseModel->setHeaderData(2, Qt::Horizontal, tr("Crop Use", ("e.g. late vs early potatoes")));
	cropUseModel->setHeaderData(3, Qt::Horizontal, tr("Harvest Unit"));
	cropUseModel->setHeaderData(4, Qt::Horizontal, "N");
	cropUseModel->setHeaderData(5, Qt::Horizontal, "K2O");
	cropUseModel->setHeaderData(6, Qt::Horizontal, "P2O5");
	cropUseModel->setHeaderData(7, Qt::Horizontal, "S");
	cropUseModel->setHeaderData(8, Qt::Horizontal, "MgO");
	cropUseModel->setHeaderData(9, Qt::Horizontal, "CaO");
	cropUseModel->setHeaderData(10, Qt::Horizontal, tr("Humus"));
	cropUseModel->setSort(3, Qt::DescendingOrder);
	cropUseModel->addTextColumn(2);
	cropUseModel->setBoolColumn(11);
	cropUseModel->select();
	ui.tableViewCropUse->setModel(cropUseModel);
	ui.tableViewCropUse->hideColumn(0);
	ui.tableViewCropUse->hideColumn(11);
	ui.tableViewCropUse->setItemDelegateForColumn(1, new QSqlRelationalDelegate(ui.tableViewCropUse));
	ui.tableViewCropUse->setItemDelegateForColumn(4, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(5, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(6, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(7, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(8, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(9, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));
	ui.tableViewCropUse->setItemDelegateForColumn(10, new SpinBoxDelegate(ui.tableViewCropUse, -999, 999, true, false, "kg"));


	connect(cropUseModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(cropUseModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	cultivarModel = new SqlRelationalTableModel(this);
	cultivarModel->setTable("cultivar");
	cultivarModel->setHeaderData(1, Qt::Horizontal, tr("Crop Use"));
	cultivarModel->setHeaderData(2, Qt::Horizontal, tr("Cultivar"));
	cultivarModel->setHeaderData(3, Qt::Horizontal, tr("Unit"));
	cultivarModel->setRelation(1, QSqlRelation("crop_use", "crop_use_id", "crop_use_name"));
	cultivarModel->setEditStrategy(SqlRelationalTableModel::OnManualSubmit);
	cultivarModel->setSort(1, Qt::AscendingOrder);
	cultivarModel->addTextColumn(2);
	cultivarModel->addTextColumn(3);
	cultivarModel->select();
	ui.tableViewCxC->setModel(cultivarModel);
	ui.tableViewCxC->hideColumn(0);
	ui.tableViewCxC->setItemDelegateForColumn(1, new QSqlRelationalDelegate(ui.tableViewCxC));

	connect(cultivarModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(cultivarModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	pltrViewModel = new SqlTableModel(this);
	pltrViewModel->setTable("v_pltr");
	pltrViewModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	pltrViewModel->setHeaderData(2, Qt::Horizontal, tr("Active Ingredient"));
	pltrViewModel->setHeaderData(3, Qt::Horizontal, tr("Type", ("e.g. fungizide")));
	pltrViewModel->setHeaderData(4, Qt::Horizontal, tr("Unit"));
	pltrViewModel->select();
	pltrViewProxy = new QSortFilterProxyModel(this);
	pltrViewProxy->setSourceModel(pltrViewModel);
	ui.tableViewPltrView->setModel(pltrViewProxy);
	ui.tableViewPltrView->hideColumn(0);

	pltrModel = new SqlTableModel(this);
	pltrModel->setTable("pltr");
	pltrModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	pltrModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	pltrModel->setHeaderData(2, Qt::Horizontal, tr("Unit"));
	pltrModel->setBoolColumn(3);
	pltrModel->addTextColumn(1);
	pltrModel->addTextColumn(2);
	pltrModel->setSort(3, Qt::DescendingOrder);
	pltrModel->select();
	ui.tableViewPltr->setModel(pltrModel);
	ui.tableViewPltr->hideColumn(0);
	ui.tableViewPltr->hideColumn(3);

	connect(pltrModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(pltrModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	pltrTypModel = new SqlTableModel(this);
	pltrTypModel->setTable("pltr_typ");
	pltrTypModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	pltrTypModel->setHeaderData(1, Qt::Horizontal, tr("Type", "e.g. fungizide"));
	pltrTypModel->addTextColumn(1);
	pltrTypModel->select();
	ui.tableViewPltrType->setModel(pltrTypModel);
	ui.tableViewPltrType->hideColumn(0);

	connect(pltrTypModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(pltrTypModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	pltrSubModel = new SqlTableModel(this);
	pltrSubModel->setTable("pltr_sub");
	pltrSubModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	pltrSubModel->setHeaderData(1, Qt::Horizontal, tr("Active Ingredient"));
	pltrSubModel->addTextColumn(1);
	pltrSubModel->select();
	ui.tableViewPltrSub->setModel(pltrSubModel);
	ui.tableViewPltrSub->hideColumn(0);

	connect(pltrSubModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(pltrSubModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));


	fertTypModel = new SqlTableModel(this);
	fertTypModel->setTable("fert_typ");
	fertTypModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	fertTypModel->setHeaderData(1, Qt::Horizontal, tr("Type", "e.g. manure, mineral fertilizer"));
	fertTypModel->addTextColumn(1);
	fertTypModel->select();
	ui.tableViewFertTyp->setModel(fertTypModel);
	ui.tableViewFertTyp->hideColumn(0);

	connect(fertTypModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(fertTypModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	fertModel = new SqlRelationalTableModel(this);
	fertModel->setTable("fert");
	fertModel->setRelation(2, QSqlRelation("fert_typ", "fert_typ_id", "fert_typ_name"));
	fertModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	fertModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	fertModel->setHeaderData(2, Qt::Horizontal, tr("Type", "type of fertilizer"));
	fertModel->setHeaderData(3, Qt::Horizontal, tr("Unit"));
	fertModel->setHeaderData(4, Qt::Horizontal, "N");
	fertModel->setHeaderData(5, Qt::Horizontal, "K2O");
	fertModel->setHeaderData(6, Qt::Horizontal, "P2O5");
	fertModel->setHeaderData(7, Qt::Horizontal, "S");
	fertModel->setHeaderData(8, Qt::Horizontal, "MgO");
	fertModel->setHeaderData(9, Qt::Horizontal, "CaO");
	fertModel->setHeaderData(10, Qt::Horizontal, "Humus");
	fertModel->setBoolColumn(11);
	fertModel->addTextColumn(1);
	fertModel->addTextColumn(3);
	fertModel->setSort(10, Qt::DescendingOrder);
	fertModel->select();
	ui.tableViewFert->setModel(fertModel);
	ui.tableViewFert->hideColumn(0);
	ui.tableViewFert->hideColumn(11);
	ui.tableViewFert->setItemDelegateForColumn(2, new QSqlRelationalDelegate(ui.tableViewFert));
	ui.tableViewFert->setItemDelegateForColumn(4, new SpinBoxDelegate(ui.tableViewFert, 0, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(5, new SpinBoxDelegate(ui.tableViewFert, 0, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(6, new SpinBoxDelegate(ui.tableViewFert, 0, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(7, new SpinBoxDelegate(ui.tableViewFert, 0, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(8, new SpinBoxDelegate(ui.tableViewFert, 0, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(9, new SpinBoxDelegate(ui.tableViewFert, -999, 999, true, false, "kg"));
	ui.tableViewFert->setItemDelegateForColumn(10, new SpinBoxDelegate(ui.tableViewFert, -999, 999, true, false, "kg"));

	connect(fertModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(fertModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	animSpcModel = new SqlTableModel(this);
	animSpcModel->setTable("anim_spc");
	animSpcModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	animSpcModel->setHeaderData(1, Qt::Horizontal, tr("Animal Types"));
	animSpcModel->setBoolColumn(2);
	animSpcModel->addTextColumn(1);
	animSpcModel->setSort(2, Qt::DescendingOrder);
	animSpcModel->select();
	ui.tableViewAnimSpc->setModel(animSpcModel);
	ui.tableViewAnimSpc->hideColumn(0);
	ui.tableViewAnimSpc->hideColumn(2);

	connect(animSpcModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(animSpcModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	envtPrgModel = new SqlTableModel(this);
	envtPrgModel->setTable("evnt_prg");
	envtPrgModel->setEditStrategy(SqlTableModel::OnManualSubmit);
	envtPrgModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	envtPrgModel->setHeaderData(2, Qt::Horizontal, tr("Abbreviation"));
	envtPrgModel->setBoolColumn(4);
	envtPrgModel->addTextColumn(1);
	envtPrgModel->addTextColumn(2);
	envtPrgModel->setSort(4, Qt::DescendingOrder);
	envtPrgModel->select();
	ui.tableViewEnvtPrg->setModel(envtPrgModel);
	ui.tableViewEnvtPrg->hideColumn(0);
	ui.tableViewEnvtPrg->hideColumn(3);
	ui.tableViewEnvtPrg->hideColumn(4);

	connect(envtPrgModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Save), SLOT(show()));
	connect(envtPrgModel, SIGNAL(dataChanged(const QModelIndex & , const QModelIndex & )),
			ui.buttonBox->button(QDialogButtonBox::Reset), SLOT(show()));

	envtPrgMapper = new QDataWidgetMapper(this);
	envtPrgMapper->setModel(envtPrgModel);
	envtPrgMapper->addMapping(ui.textEditEnvtPrgDesc, 3);
	envtPrgMapper->toFirst();
	envtPrgMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    connect(ui.tableViewEnvtPrg->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
    		envtPrgMapper, SLOT(setCurrentModelIndex(QModelIndex)));

	QSqlDatabase::database().commit();

	onSettingsChanged();
}

void BasicDataDialog::onSettingsChanged()
{
    bool alternatingRowColors = SettingsDialog::alternatingRowColors();
    QHeaderView::ResizeMode resizeMode = SettingsDialog::resizeMode();
    bool showGrid = SettingsDialog::showGrid();

    foreach (QTableView *view, findChildren<QTableView*>()) {
        view->horizontalHeader()->setResizeMode(resizeMode);
        view->horizontalHeader()->setHighlightSections(false);
        view->horizontalHeader()->setStretchLastSection(true);
        view->verticalHeader()->setHighlightSections(false);
        view->setAlternatingRowColors(alternatingRowColors);
        view->setShowGrid(showGrid);
    }
}

void BasicDataDialog::changeCurrentIndex(bool checked)
{
	if (!checked)
		return;

	if (ui.pushButtonUse->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(0);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/soiltype.png"));
		ui.toolButtonAdd->setMenu(useMenu);
	}
	if (ui.pushButtonCrop->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(1);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/crop16.png"));
		ui.toolButtonAdd->setMenu(cropMenu);
	}
	if (ui.pushButtonPltr->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr("Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(2);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/pltr16.png"));
		ui.toolButtonAdd->setMenu(pltrMenu);
	}
	if (ui.pushButtonFert->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(3);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/fert16.png"));
		ui.toolButtonAdd->setMenu(fertMenu);

	}
	if (ui.pushButtonAnim->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(4);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/cow16.png"));
		ui.toolButtonAdd->setMenu(animMenu);
	}
	if (ui.pushButtonEvntPrg->isChecked() == true) {
		if (pendingTransaction()) {
			if (QMessageBox::question(this, "CuteFarm",
		                   				   tr( "Apply changes?"),
		                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				ui.buttonBox->button(QDialogButtonBox::Save)->click();
			else
				ui.buttonBox->button(QDialogButtonBox::Reset)->click();
		}
		ui.stackedWidget->setCurrentIndex(5);
		ui.toolButtonAdd->setIcon(QIcon(":/pic/eco.png"));
		ui.toolButtonAdd->setMenu(ecoMenu);
	}
}

void BasicDataDialog::on_actionAddUse_triggered()
{
	TextInputDialog useDialog(tr("&Field Use:"), QIcon(":/pic/soiltype.png")
			, tr("Add Field Use"), this);
	start:
    if (useDialog.exec() != QDialog::Accepted)
         return;
//    qDebug() << useDialog.text();

    QString use = useDialog.text();

    if (use.isEmpty())
    	return;

	QModelIndex idx = useModel->index(0,1);
	if (!useModel->match(idx, Qt::DisplayRole, use, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(use);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = useModel->record();
	rec.setNull(0);
	rec.setValue(1, use);
	rec.setValue(2, QVariant(true));
		if (useModel->insertRecord(-1, rec)) {
			if (!useModel->submitAll()) {
				emit statusMsg(tr("Error: ") + useModel->lastError().text(), 5000);
				useModel->revertAll();
			}
		}

	if (useModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddCrop_triggered()
{
    QString cropAbbr;
    QString cropName;
    QString cropNameLat;

	CropDialog cropDialog(this);
	start:
    if (cropDialog.exec() != QDialog::Accepted)
         return;

    cropAbbr = cropDialog.cropAbbr();
    cropName = cropDialog.cropName();
    cropNameLat = cropDialog.cropNameLat();

    if (cropName.isEmpty())
    	goto start;

	if (!cropModel->match(cropModel->index(0,2), Qt::DisplayRole, cropName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(cropName);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = cropModel->record();
	rec.setNull(0);
	if (cropAbbr.isEmpty())
		rec.setNull(1);
	else
		rec.setValue(1, cropAbbr);
	rec.setValue(2, cropName);
	rec.setValue(3, cropNameLat);
	if (cropModel->insertRecord(-1, rec)) {
		if (!cropModel->submitAll()) {
			emit statusMsg(tr("Error: ") + cropModel->lastError().text(), 5000);
			cropModel->revertAll();
		}
	}

	if (cropModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddCropUse_triggered()
{
	CropUseDialog cropUseDialog(this);
	start:
    if (cropUseDialog.exec() != QDialog::Accepted)
         return;

    int cropId = cropUseDialog.cropId();
    QString cropUseName = cropUseDialog.cropUseName();
    QString unit = cropUseDialog.unit();
    double n = cropUseDialog.n();
    double k2o = cropUseDialog.k2o();
    double p2o5 = cropUseDialog.p2o5();
    double s = cropUseDialog.s();
    double mgo = cropUseDialog.mgo();
    double cao = cropUseDialog.cao();
    double humus = cropUseDialog.humus();

    if (cropUseName.isEmpty())
    	return;

	QModelIndex idx = cropUseModel->index(0,2);
	if (!cropUseModel->match(idx, Qt::DisplayRole, cropUseName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(cropUseName);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = cropUseModel->record();
	rec.setNull(0);
	if (cropId < 0)
		rec.setNull(1);
	else
		rec.setValue(1, cropId);
	rec.setValue(2, cropUseName);
	rec.setValue(3, unit);
    rec.setValue(4, n);
    rec.setValue(5, k2o);
    rec.setValue(6, p2o5);
    rec.setValue(7, s);
    rec.setValue(8, mgo);
    rec.setValue(9, cao);
    rec.setValue(10, humus);
    rec.setValue(11, "true");
	if (cropUseModel->insertRecord(-1, rec)) {
		if (!cropUseModel->submitAll()) {
			emit statusMsg(tr("Error: ") + cropUseModel->lastError().text(), 5000);
			cropUseModel->revertAll();
		}
	}

	if (cropUseModel->lastError().type() != QSqlError::NoError) {
		QSqlDatabase::database().rollback();
		cropUseModel->revertAll();
	}
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddCultivar_triggered()
{
	CultivarDialog cultivarDialog(this);

    if (cultivarDialog.exec() != QDialog::Accepted)
         return;

    int cropUseId = cultivarDialog.cropUseId();
    QString cultivarName = cultivarDialog.cultivarName();
    QString seedUnit = cultivarDialog.seedUnit();

    if (cropUseId < 0 || cultivarName.isEmpty())
    	return;

	QSqlDatabase::database().transaction();

	QSqlRecord rec = cultivarModel->record();
	rec.setNull(0);
	rec.setValue(1, cropUseId);
	rec.setValue(2, cultivarName);
	rec.setValue(3, seedUnit);
	if (cultivarModel->insertRecord(-1, rec)) {
		if (!cultivarModel->submitAll()) {
			emit statusMsg(tr("Error: ") + cultivarModel->lastError().text(), 5000);
			cultivarModel->revertAll();
		}
	}

	if (cultivarModel->lastError().type() != QSqlError::NoError) {
		QSqlDatabase::database().rollback();
		cultivarModel->revertAll();
	}
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddPltr_triggered()
{
	PltrDialog psmDialog(this);
	start:
    if (psmDialog.exec() != QDialog::Accepted)
         return;

    int pltrId = psmDialog.pltrId();
    QString pltrName = psmDialog.pltrName();
    QString pltrUnit = psmDialog.pltrUnit();
    QList<int> pltrTypIds = psmDialog.pltrTypIds();
    QList<int> pltrSubIds = psmDialog.pltrSubIds();

    if (pltrId < 0 || pltrName.isEmpty() || pltrName.isEmpty() || pltrUnit.isEmpty() || pltrSubIds.isEmpty()
            || pltrTypIds.isEmpty())
    	goto start;

	QSqlDatabase::database().transaction();

	QSqlRecord rec = pltrModel->record();
	rec.setValue(0, pltrId);
	rec.setValue(1, pltrName);
	rec.setValue(2, pltrUnit);
	rec.setValue(3, QVariant(true));
    if (pltrModel->insertRecord(-1, rec)) {
        if (!pltrModel->submitAll()) {
            emit statusMsg(tr("Error: ") + pltrModel->lastError().text(), 5000);
            pltrModel->revertAll();
        }
        qDebug() << pltrModel->lastError().text();
    }

	QSqlQuery q;
	bool error = false;
	foreach (int pltrTypId, pltrTypIds) {
		q.prepare(" INSERT INTO pltr_x_pltr_typ "
				  " VALUES(NULL, :pltrId, :pltrTypId) ");
		q.bindValue(":pltrId", pltrId);
		q.bindValue(":pltrTypId", pltrTypId);
		q.exec();
		if (q.lastError().type() != QSqlError::NoError) {
			error = true;
			qDebug() <<  q.lastError().text();

		}
	}

	foreach (int pltrSubId, pltrSubIds) {
		q.prepare(" INSERT INTO pltr_x_pltr_sub "
				  " VALUES(NULL, :pltrId, :pltrSubId) ");
		q.bindValue(":pltrId", pltrId);
		q.bindValue(":pltrSubId", pltrSubId);
		q.exec();
		if (q.lastError().type() != QSqlError::NoError) {
			error = true;
			qDebug() << q.lastError().text();
		}
	}

	if (pltrModel->lastError().type() != QSqlError::NoError || error == true) {
		QSqlDatabase::database().rollback();
		qDebug() << pltrModel->lastError().text();
		qDebug() << q.lastError().text();

	}
	else {
		QSqlDatabase::database().commit();
		pltrViewModel->select();
	}

}

void BasicDataDialog::on_actionEditPltr_triggered()
{
    const QModelIndex idx = ui.tableViewPltr->currentIndex();

    if (!idx.isValid())
        return;

    int pltrId = pltrModel->data(idx.sibling(idx.row(), 0), Qt::DisplayRole).toInt();
    QString pltrName = pltrModel->data(idx.sibling(idx.row(), 1), Qt::DisplayRole).toString();
    QString pltrUnit = pltrModel->data(idx.sibling(idx.row(), 2), Qt::DisplayRole).toString();

    if (pltrId < 0)
        return;

    PltrDialog psmDialog(this, pltrName, pltrUnit, pltrId);
    start:
    if (psmDialog.exec() != QDialog::Accepted)
         return;

    pltrName = psmDialog.pltrName();
    pltrUnit = psmDialog.pltrUnit();
    QList<int> pltrTypIds = psmDialog.pltrTypIds();
    QList<int> pltrSubIds = psmDialog.pltrSubIds();

    if (pltrId < 0 || pltrName.isEmpty() || pltrName.isEmpty() || pltrUnit.isEmpty() || pltrSubIds.isEmpty()
            || pltrTypIds.isEmpty())
        goto start;

    QSqlDatabase::database().transaction();

    QSqlQuery q;
    q.prepare("UPDATE pltr SET pltr_name=:pltr_name, pltr_unit=:pltr_unit "
            "WHERE pltr_id=:pltr_id");
    q.bindValue(":pltr_name", pltrName);
    q.bindValue(":pltr_unit", pltrUnit);
    q.bindValue(":pltr_id", pltrId);
    q.exec();

    q.prepare("DELETE FROM pltr_x_pltr_sub "
            "WHERE pltr_id=:pltr_id");
    q.bindValue(":pltr_id", pltrId);
    q.exec();

    q.prepare("DELETE FROM pltr_x_pltr_typ "
            "WHERE pltr_id=:pltr_id");
    q.bindValue(":pltr_id", pltrId);
    q.exec();


    bool error = false;
    foreach (int pltrTypId, pltrTypIds) {
        q.prepare(" INSERT INTO pltr_x_pltr_typ "
                  " VALUES(NULL, :pltrId, :pltrTypId) ");
        q.bindValue(":pltrId", pltrId);
        q.bindValue(":pltrTypId", pltrTypId);
        q.exec();
        if (q.lastError().type() != QSqlError::NoError) {
            error = true;
            qDebug() << q.lastError().text();

        }
    }

    foreach (int pltrSubId, pltrSubIds) {
        q.prepare(" INSERT INTO pltr_x_pltr_sub "
                  " VALUES(NULL, :pltrId, :pltrSubId) ");
        q.bindValue(":pltrId", pltrId);
        q.bindValue(":pltrSubId", pltrSubId);
        q.exec();
        if (q.lastError().type() != QSqlError::NoError) {
            error = true;
            qDebug() << q.lastError().text();
        }
    }

    if (pltrModel->lastError().type() != QSqlError::NoError || error == true) {
        QSqlDatabase::database().rollback();
        qDebug() << pltrModel->lastError().text();
        qDebug() << q.lastError().text();

    }
    else {
        QSqlDatabase::database().commit();
        pltrModel->select();
        pltrViewModel->select();
    }
}

void BasicDataDialog::on_actionAddPltrTyp_triggered()
{
	TextInputDialog pltrTypDialog(tr("&Planttreatment Type:"), QIcon(":/pic/pltr16.png")
			, tr("Add Planttreatment Type"), this);
	start:
    if (pltrTypDialog.exec() != QDialog::Accepted)
         return;

    QString pltrTyp = pltrTypDialog.text();

    if (pltrTyp.isEmpty())
    	goto start;

    QSqlDatabase::database().transaction();

	QSqlRecord rec = pltrTypModel->record();
	rec.setNull(0);
	rec.setValue(1, pltrTyp);
		if (pltrTypModel->insertRecord(-1, rec)) {
			if (!pltrTypModel->submitAll()) {
				emit statusMsg(tr("Error: ") + pltrTypModel->lastError().text(), 5000);
				pltrTypModel->revertAll();
			}
		}

	if (pltrTypModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();
}

void BasicDataDialog::on_actionAddPltrSub_triggered()
{
	TextInputDialog pltrSubDialog(tr("&Active Ingredient:"), QIcon(":/pic/pltr16.png")
			, tr("Add Active Ingredient"), this);
	start:
    if (pltrSubDialog.exec() != QDialog::Accepted)
         return;

    QString pltrSub = pltrSubDialog.text();

    if (pltrSub.isEmpty())
    	return;

	QModelIndex idx = pltrSubModel->index(0,1);
	if (!pltrSubModel->match(idx, Qt::DisplayRole, pltrSub, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(pltrSub);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = pltrSubModel->record();
	rec.setNull(0);
	rec.setValue(1, pltrSub);
		if (pltrSubModel->insertRecord(-1, rec)) {
			if (!pltrSubModel->submitAll()) {
				emit statusMsg(tr("Error: ") + pltrSubModel->lastError().text(), 5000);
				pltrSubModel->revertAll();
			}
		}

	if (pltrSubModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddFert_triggered()
{
	FertDialog fertDialog(this);
	start:
    if (fertDialog.exec() != QDialog::Accepted)
         return;

//    int fertId = fertDialog.fertId();
    QString fertName = fertDialog.fertName();
    int fertTypId = fertDialog.fertTypId();
    QString fertTypName = fertDialog.fertTypName();
    QString fertUnit = fertDialog.fertUnit();
    double n = fertDialog.n();
    double k2o = fertDialog.k2o();
    double p2o5 = fertDialog.p2o5();
    double s = fertDialog.s();
    double mgo = fertDialog.mgo();
    double cao = fertDialog.cao();
    double humus = fertDialog.humus();

//    qDebug() << fertId << fertName << fertUnit << fertTypId << fertTypName << n << k2o << p2o5 << s << mgo << cao;


    if (fertName.isEmpty() || fertTypName.isEmpty() || fertUnit.isEmpty())
    	return;

	QModelIndex idx = fertModel->index(0,1);
	if (!fertModel->match(idx, Qt::DisplayRole, fertName, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(fertName);
		goto start;
		}

	QSqlDatabase::database().transaction();

	if (fertDialog.newFertTyp()) {
		QSqlRecord rec = fertTypModel->record();
		rec.setValue(0, fertTypId);
		rec.setValue(1, fertTypName);
		if (fertTypModel->insertRecord(-1, rec)) {
			if (!fertTypModel->submitAll()) {
				emit statusMsg(tr("Error: ") + fertTypModel->lastError().text(), 5000);
				fertTypModel->revertAll();
			}
		}
	}

	QSqlRecord rec = fertModel->record();

//	qDebug() << rec.value(2) << rec.fieldName(2);
	rec.setNull(0);
	rec.setValue(1, fertName);
	rec.setValue(2, fertTypId);
	rec.setValue(3, fertUnit);
	rec.setValue(4, n);
	rec.setValue(5, k2o);
	rec.setValue(6, p2o5);
	rec.setValue(7, s);
	rec.setValue(8, mgo);
	rec.setValue(9, cao);
	rec.setValue(10, humus);
	rec.setValue(11, QVariant(true));
	if (fertModel->insertRecord(-1, rec)) {
		if (!fertModel->submitAll()) {
			emit statusMsg(tr("Error: ") + fertModel->lastError().text(), 5000);
			fertModel->revertAll();
		}
	}

	if (fertTypModel->lastError().type() != QSqlError::NoError || fertModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddAnim_triggered()
{
	TextInputDialog animDialog(tr("&Animal Species:"), QIcon(":/pic/cow.png")
			, tr("Add Animal Species"), this);
	start:
    if (animDialog.exec() != QDialog::Accepted)
         return;

    QString anim = animDialog.text();

    if (anim.isEmpty())
    	return;

	QModelIndex idx = animSpcModel->index(0,1);
	if (!animSpcModel->match(idx, Qt::DisplayRole, anim, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(anim);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = animSpcModel->record();
	rec.setNull(0);
	rec.setValue(1, anim);
	rec.setValue(2, QVariant(true));
		if (animSpcModel->insertRecord(-1, rec)) {
			if (!animSpcModel->submitAll()) {
				emit statusMsg(tr("Error: ") + animSpcModel->lastError().text(), 5000);
				animSpcModel->revertAll();
			}
		}

	if (animSpcModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_actionAddEvntPrg_triggered()
{
	EvntPrgDialog evntPrgDialog(this);
	start:
    if (evntPrgDialog.exec() != QDialog::Accepted)
         return;

    QString name = evntPrgDialog.name();
    QString abbr = evntPrgDialog.abbr();

//    qDebug() << name << abbr;

    if (name.isEmpty() || abbr.isEmpty())
        goto start;

	QModelIndex idx = envtPrgModel->index(0,1);
	if (!envtPrgModel->match(idx, Qt::DisplayRole, name, 1,
			Qt::MatchFixedString).isEmpty()) {
		identMsg(name);
		goto start;
		}

	QSqlDatabase::database().transaction();

	QSqlRecord rec = envtPrgModel->record();
	rec.setNull(0);
	rec.setValue(1, name);
	rec.setValue(2, abbr);
	rec.setValue(3, QString());
	rec.setValue(4, QVariant(true));

		if (envtPrgModel->insertRecord(-1, rec)) {
			if (!envtPrgModel->submitAll()) {
				emit statusMsg(tr("Error: ") + envtPrgModel->lastError().text(), 5000);
				envtPrgModel->revertAll();
			}
		}

	if (envtPrgModel->lastError().type() != QSqlError::NoError)
		QSqlDatabase::database().rollback();
	else
		QSqlDatabase::database().commit();

}

void BasicDataDialog::on_buttonBox_helpRequested()
{
    QString message;

    switch (ui.stackedWidget->currentIndex()) {
    case 0:
        message = tr("Each field has a specific 'Field Uses' e.g. arable land, pasture etc.");
        break;
    case 1:
        message = tr("A crop abbreviation is optional. Each crop is linked to one or more usages "
        		"e.g. a late and an early maturing potato. The 'Crop Use' is linked to cultivars.\n"
        		"Withdrawal is usually negative. In case of N fixation use positive figures. "
        		"Nutrient withdrawal must be in %1/harvest unit!.").arg(SettingsDialog::nutUnit());
        break;
    case 2:
        message = tr("Plant Treatments must contain one or more active ingredients and must be of one or more types.");
        break;
    case 3:
        message = tr("A fertilizer contains nutrients in %1/unit!. CuteFarm assume that the unit used here "
        		"is the same unit used when fertilizers are applied in operations.").arg(SettingsDialog::nutUnit());
        break;
    case 4:
        message = tr("In order to add animal groups you have to add types of animal groups here.");
        break;
    case 5:
        message = tr("Environment schemes you are participating in. You may add a description as well.");
        break;
    default:
        message = "This help is likely not to be of any help.";
    }

    QMessageBox::information(this, tr("General Data", "opposite of farm specific data"), message);
}

QTableView* BasicDataDialog::currentTable()
{
	if (ui.tableViewAnimSpc->isVisible()) {
//		qDebug() << "tableViewAnimSpc";
		return ui.tableViewAnimSpc;
	}
	else if (ui.tableViewCrop->isVisible()) {
//		qDebug() << "tableViewCrop";
		return ui.tableViewCrop;
	}
	else if (ui.tableViewCropUse->isVisible()) {
//		qDebug() << "tableViewCrop";
		return ui.tableViewCropUse;
	}
	else if (ui.tableViewCxC->isVisible()) {
//		qDebug() << "tableViewCxC";
		return ui.tableViewCxC;
	}
	else if (ui.tableViewEnvtPrg->isVisible()) {
//		qDebug() << "tableViewEnvtPrg";
		return ui.tableViewEnvtPrg;
	}
	else if (ui.tableViewFert->isVisible()) {
//		qDebug() << "tableViewFert";
		return ui.tableViewFert;
	}
	else if (ui.tableViewFertTyp->isVisible()) {
//		qDebug() << "tableViewFertTyp";
		return ui.tableViewFertTyp;
	}
	else if (ui.tableViewPltr->isVisible()) {
//		qDebug() << "tableViewPltr";
		return ui.tableViewPltr;
	}
	else if (ui.tableViewPltrSub->isVisible()) {
//		qDebug() << "tableViewPltrSub";
		return ui.tableViewPltrSub;
	}
	else if (ui.tableViewPltrType->isVisible()) {
//		qDebug() << "tableViewPltrType";
		return ui.tableViewPltrType;
	}
	else if (ui.tableViewUse->isVisible()) {
//		qDebug() << "tableViewUse";
		return ui.tableViewUse;
	}

	return 0;
}

void BasicDataDialog::on_toolButtonRemove_clicked()
{

	QTableView *view = currentTable();
	if (view) {
		SqlTableModel *model = qobject_cast<SqlTableModel*>(view->model());
//		qDebug() << model;
		if (model) {
			if (view->currentIndex().isValid()) {
				QString data = view->currentIndex().sibling(view->currentIndex().row(), 1).data().toString();
//				qDebug() << data;
				QList<QModelIndex> idxList;
//				qDebug() << idxList;
				idxList = view->selectionModel()->selectedRows();
				if (idxList.isEmpty())
					return;
				if (idxList.count() > 1)
					data = QString();
				if (removeMsg(data)) {
			         for (int i = 0; i < idxList.size(); ++i) {
			        	 if (model->removeRow(idxList.at(i).row(), QModelIndex())) {
			        		 ui.buttonBox->button(QDialogButtonBox::Reset)->show();
							 ui.buttonBox->button(QDialogButtonBox::Save)->show();
						 }
			         }
			         model->setPendingTransaction(true);
				}
			}
		}
		else {
			SqlRelationalTableModel *rmodel = qobject_cast<SqlRelationalTableModel*>(view->model());
			if (rmodel) {
				if (view->currentIndex().isValid()) {
				    QString data = view->currentIndex().sibling(view->currentIndex().row(), 1).data().toString();
					QList<QModelIndex> idxList;
					idxList = view->selectionModel()->selectedRows();
					if (idxList.isEmpty())
						return;
					if (idxList.count() > 1)
						data = QString();
					if (removeMsg(data)) {
				         for (int i = 0; i < idxList.size(); ++i) {
				        	 if (rmodel->removeRow(idxList.at(i).row(), QModelIndex())) {
				        		ui.buttonBox->button(QDialogButtonBox::Reset)->show();
								ui.buttonBox->button(QDialogButtonBox::Save)->show();
							}
				        }
				        rmodel->setPendingTransaction(true);
					}
				}
			}
		}
	}
}

void BasicDataDialog::on_toolButtonIsActive_clicked()
{
	QTableView *view = currentTable();
	if (view) {
		SqlTableModel *model = qobject_cast<SqlTableModel*>(view->model());
//		qDebug() << model;
		if (model) {
			if (model->boolColumn() < 0)
				return;
			QList<QModelIndex> idxList;
			idxList = view->selectionModel()->selectedRows();
			if (idxList.isEmpty())
				return;
		    for (int i = 0; i < idxList.size(); ++i) {
		    	bool data = model->data(idxList.at(i).sibling(idxList.at(i).row(), model->boolColumn())).toBool();
		    	if (data)
		    		data = false;
		    	else
		    		data = true;
		    	model->setData(idxList.at(i).sibling(idxList.at(i).row(), model->boolColumn()), QVariant(data));
		    }
		    model->setPendingTransaction(true);
		}
		else {
			SqlRelationalTableModel *rmodel = qobject_cast<SqlRelationalTableModel*>(view->model());
			if (rmodel) {
				if (rmodel->boolColumn() < 0)
					return;
				QList<QModelIndex> idxList;
				idxList = view->selectionModel()->selectedRows();
				if (idxList.isEmpty())
					return;
			    for (int i = 0; i < idxList.size(); ++i) {
			    	bool data = rmodel->data(idxList.at(i).sibling(idxList.at(i).row(), rmodel->boolColumn())).toBool();
			    	if (data)
			    		data = false;
			    	else
			    		data = true;
			    	rmodel->setData(idxList.at(i).sibling(idxList.at(i).row(), rmodel->boolColumn()), QVariant(data));
			    }
			    rmodel->setPendingTransaction(true);
			}
		}
	}
}

bool BasicDataDialog::pendingTransaction()
{
	if ( useModel->pendingTransaction() || envtPrgModel->pendingTransaction() ||
			cropModel->pendingTransaction() || cultivarModel->pendingTransaction() || pltrModel->pendingTransaction() ||
			pltrTypModel->pendingTransaction() || pltrSubModel->pendingTransaction() || fertModel->pendingTransaction() ||
			fertTypModel->pendingTransaction() || animSpcModel->pendingTransaction())
		return true;
	else
		return false;
}

QString BasicDataDialog::submit(QAbstractItemModel *model)
{
    SqlRelationalTableModel *rm = qobject_cast<SqlRelationalTableModel*>(model);
    SqlTableModel *m = qobject_cast<SqlTableModel*>(model);
    QString err;

    if (rm) {
        QSqlDatabase::database().transaction();
        if(!rm->submitAll()) {
            err.append(QString("%1\n").arg(rm->lastError().text()));
            QSqlDatabase::database().rollback();
            rm->revertAll();
            rm->setPendingTransaction(false);
        }
        else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            rm->select();
        }
    }
    else if (m) {
        QSqlDatabase::database().transaction();
        if(!m->submitAll()) {
            err.append(QString("%1\n").arg(m->lastError().text()));
            QSqlDatabase::database().rollback();
            m->revertAll();
            m->setPendingTransaction(false);
        }
        else if(!QSqlDatabase::database().commit()) {
            err.append(QString("%1\n").arg(QSqlDatabase::database().lastError().text()));
            QSqlDatabase::database().rollback();
            m->select();
        }
    }

    return err;
}

void BasicDataDialog::buttonClose()
{
	if(ui.buttonBox->button(QDialogButtonBox::Save)->isVisible()){
		if(QMessageBox::question(0, "CuteFarm",
	                   tr("Apply changes?"),
	                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			buttonSave();
			done(0);
		}
		else{
//			buttonReset();
			done(0);
		}
	}
	else{
//		buttonReset();
		done(0);
	}
}

void BasicDataDialog::buttonSave()
{
	QString err;

	if (useModel->pendingTransaction())
	    err.append(submit(useModel));
    if (cropModel->pendingTransaction())
        err.append(submit(cropModel));
    if (cropUseModel->pendingTransaction())
        err.append(submit(cropUseModel));
    if (cultivarModel->pendingTransaction())
        err.append(submit(cultivarModel));
    if (pltrModel->pendingTransaction())
        err.append(submit(pltrModel));
    if (pltrTypModel->pendingTransaction())
        err.append(submit(pltrTypModel));
    if (pltrSubModel->pendingTransaction())
        err.append(submit(pltrSubModel));
    if (fertModel->pendingTransaction())
        err.append(submit(fertModel));
    if (fertTypModel->pendingTransaction())
        err.append(submit(fertTypModel));
    if (animSpcModel->pendingTransaction())
        err.append(submit(animSpcModel));
    if (envtPrgModel->pendingTransaction())
        err.append(submit(envtPrgModel));

	if (!err.isEmpty())
		QMessageBox::warning(this , "CuteFarm", tr("Error:\n") + err);
	else
		emit statusMsg("Ok", 5000);

	ui.buttonBox->button(QDialogButtonBox::Reset)->hide();
	ui.buttonBox->button(QDialogButtonBox::Save)->hide();
}

void BasicDataDialog::buttonReset()
{
	if (useModel->pendingTransaction()) {
		useModel->revertAll();
		useModel->setPendingTransaction(false);
	}
	if (cropModel->pendingTransaction()) {
		cropModel->revertAll();
		cropModel->setPendingTransaction(false);
	}
	if (cultivarModel->pendingTransaction()) {
		cultivarModel->revertAll();
		cultivarModel->setPendingTransaction(false);
	}
	if (pltrModel->pendingTransaction()) {
		pltrModel->revertAll();
		pltrModel->setPendingTransaction(false);
	}
	if (pltrTypModel->pendingTransaction()) {
		pltrTypModel->revertAll();
		pltrTypModel->setPendingTransaction(false);
	}
	if (pltrSubModel->pendingTransaction()) {
		pltrSubModel->revertAll();
		pltrSubModel->setPendingTransaction(false);
	}
	if (fertModel->pendingTransaction()) {
		fertModel->revertAll();
		fertModel->setPendingTransaction(false);
	}
	if (fertTypModel->pendingTransaction()) {
		fertTypModel->revertAll();
		fertTypModel->setPendingTransaction(false);
	}
	if (animSpcModel->pendingTransaction()) {
		animSpcModel->revertAll();
		animSpcModel->setPendingTransaction(false);
	}
	if (envtPrgModel->pendingTransaction()) {
		envtPrgModel->revertAll();
		envtPrgModel->setPendingTransaction(false);
	}

	ui.buttonBox->button(QDialogButtonBox::Reset)->hide();
	ui.buttonBox->button(QDialogButtonBox::Save)->hide();
}

bool BasicDataDialog::removeMsg(const QString &data)
{
	if(!data.isEmpty()){
		if(QMessageBox::question(this, "CuteFarm",
	                   tr("If this entry is connected to e.g. an action\n"
	                	"it is not possible to delete it.\n"
	                      "\n"
	                      "Delete '%1'?").arg(data),
		                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			return true;
		}else{
			return false;
		}
	}
	else{
		if(QMessageBox::question(this, "CuteFarm",
	                   tr("If this entry is connected to e.g. an action\n"
	                	"it is not possible to delete it.\n"
	                      "\n"
	                      "Delete selected rows?"),
		                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
			return true;
		}else{
			return false;
		}
	}
}

void BasicDataDialog::identMsg(const QString &name)
{
	QMessageBox::information(0, "CuteFarm",
	                   tr("The name '%1' is already existing.",
	                		   "message to indicate that duplicate names e.g. for crops are not allowed.").arg(name.simplified()),
	                   QMessageBox::Ok);
}

BasicDataDialog::~BasicDataDialog()
{

}
