#ifndef NUMBERINPUTDIALOG_H
#define NUMBERINPUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_numberinputdialog.h"

class NumberInputDialog : public QDialog
{
    Q_OBJECT

public:
    NumberInputDialog(QWidget *parent = 0, QString title = QString(), QString info = QString(), QString text = QString());
    ~NumberInputDialog();

    int number() const;

private:
    Ui::NumberInputDialogClass ui;
};

#endif // NUMBERINPUTDIALOG_H
