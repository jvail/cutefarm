#include "evntprgdialog.h"

EvntPrgDialog::EvntPrgDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

QString EvntPrgDialog::name() const
{
	return ui.lineEditName->text();
}

QString EvntPrgDialog::abbr() const
{
	return ui.lineEditAbbr->text();
}

EvntPrgDialog::~EvntPrgDialog()
{

}
