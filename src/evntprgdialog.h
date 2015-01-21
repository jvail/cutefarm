#ifndef EVNTPRGDIALOG_H
#define EVNTPRGDIALOG_H

#include <QtGui/QDialog>
#include "ui_evntprgdialog.h"

class EvntPrgDialog : public QDialog
{
    Q_OBJECT

public:
    EvntPrgDialog(QWidget *parent = 0);
    ~EvntPrgDialog();
    
    QString name() const;
    QString abbr() const;

private:
    Ui::EvntPrgDialogClass ui;
};

#endif // EVNTPRGDIALOG_H
