#ifndef NEWHARVESTYEARDIALOG_H
#define NEWHARVESTYEARDIALOG_H

#include <QtGui/QDialog>
#include <QStandardItemModel>

#include "ui_newharvestyeardialog.h"

class NewHarvestYearDialog : public QDialog
{
    Q_OBJECT

public:
    NewHarvestYearDialog(int oldYear, QWidget *parent = 0);
    ~NewHarvestYearDialog();

    QSet<int> fildUseIds() const;

private:
	QStandardItemModel *m_fildUseModel;
    Ui::NewHarvestYearDialogClass ui;
};

#endif // NEWHARVESTYEARDIALOG_H
