#include "numberinputdialog.h"

NumberInputDialog::NumberInputDialog(QWidget *parent, QString title, QString info, QString text)
    : QDialog(parent)
{
	ui.setupUi(this);
	setWindowIcon(parentWidget()->windowIcon());
	setWindowTitle(title);
	ui.label->setText(text);
	ui.labelInfo->setText(info);
}

int NumberInputDialog::number() const
{
	return ui.spinBox->value();
}

NumberInputDialog::~NumberInputDialog()
{

}
