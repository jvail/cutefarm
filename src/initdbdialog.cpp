
#include <QFile>

#include "initdbdialog.h"


InitDbDialog::InitDbDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QApplication::restoreOverrideCursor();

	if (QFile::exists(":/sql/de_crop.sql"))
		ui.comboBoxCrops->insertItem(0, tr("German Croplist"), ":/sql/de_crop.sql");
	if (QFile::exists(":/sql/fr_crop.sql"))
		ui.comboBoxCrops->insertItem(1, tr("French Croplist"), ":/sql/fr_crop.sql");

}

QString InitDbDialog::cropFile() const
{
	if (ui.checkBoxCrops->isChecked() && ui.comboBoxCrops->count() > 0)
	{
		return ui.comboBoxCrops->model()->data(ui.comboBoxCrops->model()->index(ui.comboBoxCrops->currentIndex(),0),
				Qt::UserRole).toString();
	}
	else
		return QString();
}

InitDbDialog::~InitDbDialog()
{

}
