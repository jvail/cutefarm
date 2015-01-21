#ifndef INITDBDIALOG_H
#define INITDBDIALOG_H

#include <QtGui/QDialog>
#include "ui_initdbdialog.h"

class InitDbDialog : public QDialog
{
    Q_OBJECT

public:
    InitDbDialog(QWidget *parent = 0);
    ~InitDbDialog();

    QString cropFile() const;

private:
    Ui::InitDbDialogClass ui;
};

#endif // INITDBDIALOG_H
