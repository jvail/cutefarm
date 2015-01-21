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
#include <QDebug>
#include <QTextCodec>
#include <QSqlQuery>
#include <QDir>
#include <QFileDialog>

#include "settingsdialog.h"

QSettings SettingsDialog::mSettings("CuteFarm");

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QString lotText =
	        tr("I don't need lots.\n"
	        "Hides all lot widgets and automatically adds a dummy lot of the same size and connects it to the field when a new field is added.\n"
	        "If you delete a field all now unallocated lots will  be deleted automatically!");

	ui.labeLots->setText(lotText);

//	QVariant w = height() * 1.33;
//	resize(w.toInt(), height());

	ui.checkBoxRowColor->setChecked(mSettings.value("alternatingRowColors", true).toBool());
	ui.checkBoxGrid->setChecked(mSettings.value("showGrid", true).toBool());
	ui.radioButtonStretch->setChecked(mSettings.value("resizeModeStretch", false).toBool());
	ui.radioButtonContents->setChecked(mSettings.value("resizeModeContents", true).toBool());
	ui.dbPathLineEdit->setText(mSettings.value("pathToDatabase").toString());
	ui.checkBoxNoLots->setChecked(mSettings.value("noLots", true).toBool());
	ui.areaLineEdit->setText(mSettings.value("areaUnit", "ha").toString());
	ui.nutLineEdit->setText(mSettings.value("nutUnit", "kg").toString());

	QStringList styles = QStyleFactory::keys();
    //remove ugly styles
	if (styles.count() > 1)
	    styles.removeOne("CDE");
    if (styles.count() > 1)
        styles.removeOne("Windows");
    if (styles.count() > 1)
        styles.removeOne("WindowsXP");
    if (styles.count() > 1)
        styles.removeOne("Motif");

	ui.comboBoxStyle->addItems(styles);
	ui.comboBoxStyle->setCurrentIndex(
	        ui.comboBoxStyle->findData(SettingsDialog::style(), Qt::DisplayRole, Qt::MatchFixedString));

	loadTranslations();

	ui.comboBoxLocale->setCurrentIndex(ui.comboBoxLocale->findData(SettingsDialog::locale()));
    if (ui.comboBoxLocale->currentIndex() < 0)
        ui.comboBoxLocale->setCurrentIndex(ui.comboBoxLocale->findData("en_US"));

    connect(ui.buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
            this, SLOT(restoreDefaults()));
    connect(ui.comboBoxStyle, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateStyle(QString)));
}

void SettingsDialog::on_buttonBox_accepted()
{
	mSettings.setValue("alternatingRowColors", ui.checkBoxRowColor->isChecked());
	mSettings.setValue("showGrid", ui.checkBoxGrid->isChecked());
	mSettings.setValue("resizeModeStretch", ui.radioButtonStretch->isChecked());
	mSettings.setValue("resizeModeContents", ui.radioButtonContents->isChecked());
	mSettings.setValue("pathToDatabase", QDir::toNativeSeparators(ui.dbPathLineEdit->text()));
	QString locale = ui.comboBoxLocale->model()->data(ui.comboBoxLocale->model()->index(ui.comboBoxLocale->currentIndex(),0),
					Qt::UserRole).toString();
	mSettings.setValue("locale", locale);
	mSettings.setValue("style", ui.comboBoxStyle->currentText());
	mSettings.setValue("noLots", ui.checkBoxNoLots->isChecked());
	mSettings.setValue("areaUnit", ui.areaLineEdit->text());
	mSettings.setValue("nutUnit", ui.nutLineEdit->text());

	emit settingsChanged();
//	QApplication::setStyle(ui.comboBoxStyle->currentText());
}

void SettingsDialog::on_buttonBox_rejected()
{
	QApplication::setStyle(mSettings.value("style", "Plastique").toString());
}

bool SettingsDialog::alternatingRowColors()
{
	return mSettings.value("alternatingRowColors", true).toBool();
}

enum QHeaderView::ResizeMode SettingsDialog::resizeMode()
{
	return (mSettings.value("resizeModeStretch", true).toBool()) ? QHeaderView::Stretch : QHeaderView::Interactive;
}

bool SettingsDialog::showGrid()
{
	return mSettings.value("showGrid", true).toBool();
}

QString SettingsDialog::style()
{
    QStringList styles = QStyleFactory::keys();

#ifdef Q_WS_X11
    QString style = (styles.contains("oxygen", Qt::CaseInsensitive)) ? "oxygen" : "plastique";
#else
    QString style = (styles.contains("windowsvista", Qt::CaseInsensitive)) ? "windowsvista" : "plastique";
#endif

	return mSettings.value("style", style).toString();
}

QString SettingsDialog::locale()
{
	return mSettings.value("locale", QLocale::system().name()).toString();
}

bool SettingsDialog::lotsDisabled()
{
	return mSettings.value("noLots", true).toBool();
}

QString SettingsDialog::areaUnit()
{
    return mSettings.value("areaUnit", "ha").toString();
}

QString SettingsDialog::nutUnit()
{
    return mSettings.value("nutUnit", "kg").toString();
}

void SettingsDialog::restoreDefaults()
{

	ui.checkBoxRowColor->setChecked(true);
	ui.checkBoxGrid->setChecked(false);
	ui.radioButtonStretch->setChecked(true);
	QString path = QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "CuteFarm" + QDir::separator()) + "cutefarm.sqlite3";
	ui.dbPathLineEdit->setText(path);
	ui.areaLineEdit->setText("ha");
	ui.nutLineEdit->setText("kg");

    QStringList styles = QStyleFactory::keys();

#ifdef Q_WS_X11
    QString style = (styles.contains("oxygen"), Qt::CaseInsensitive) ? "oxygen" : "plastique";
#else
    QString style = (styles.contains("windowsvista"), Qt::CaseInsensitive) ? "windowsvista" : "plastique";
#endif

	ui.comboBoxStyle->setCurrentIndex(ui.comboBoxStyle->findData(style, Qt::DisplayRole, Qt::MatchFixedString));

	ui.comboBoxLocale->setCurrentIndex(ui.comboBoxLocale->model()->match(
			ui.comboBoxLocale->model()->index(0, 0), Qt::UserRole, SettingsDialog::locale()).first().row());

	ui.checkBoxNoLots->setChecked(true);
}

void SettingsDialog::on_dbPathPushButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
					     tr("Database"), QDir::homePath(), "SQLite (*.sqlite3)");
	if (!path.isEmpty())
			ui.dbPathLineEdit->setText(path);

}

void SettingsDialog::updateStyle(const QString &style)
{
	QApplication::setStyle(style);
}

void SettingsDialog::loadTranslations()
{
	// cutefarm_<language>_<country>.qm
	QDir dir(":/trs");
	QString filter = "cutefarm_*.qm";
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort = QDir::Name;
	QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);

	foreach (const QFileInfo &file, entries) {
		QStringList parts = file.baseName().split("_");
		if (parts.count() < 2)
		    continue;
		QString language = parts.at(1).toLower();
		QLocale locale(language);

		if (locale.name() != "C") {
			QString language = QLocale::languageToString(locale.language());
			QString country = QLocale::countryToString(locale.country());
			QString name = language + " (" + country + ")";
			ui.comboBoxLocale->addItem(name, locale.name());
		}
	}

	QLocale locale("en");
	QString language = QLocale::languageToString(locale.language());
	QString country = QLocale::countryToString(locale.country());
	QString name = language + " (" + country + ")";
	ui.comboBoxLocale->addItem(name, locale.name());
}

SettingsDialog::~SettingsDialog()
{

}
