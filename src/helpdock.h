#ifndef HELPDOCK_H
#define HELPDOCK_H

#include <QtGui/QWidget>
#include "ui_helpdock.h"

class HelpDock : public QDockWidget
{
    Q_OBJECT

public:
    HelpDock(QWidget *parent = 0);
    ~HelpDock();

private:

    Ui::HelpDockClass ui;
};

#endif // HELPDOCK_H
