
#include <QSqlQuery>

#include "aboutdlg.h"
#include "../cutefarm.h"

AboutDlg::AboutDlg(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QString title = QString("CuteFarm %1").arg(CuteFarm::CUTEFARM_BUILD);
	QString descr = tr("CuteFarm is a farm management application. Keep records of farming activities and agricultural resources.");
	QString copy = "Copyright © 2008 - 2011 by Jan Vaillant (jvail@gmx.de)";
    QString version = "SQLite ";
    QSqlQuery q;
    q.exec("SELECT sqlite_version()");
    while (q.next())
        version.append(q.value(0).toString());

    version.append(" / ").append("Qt ").append(qVersion());

    QString credits =
            "Icons Tulliana 2.0 "+tr("by")+ " M.Umut Pulat<br/>"
            "Logo 'Ladybug' "+tr("by")+ " Ganzio Design<br/><br/>" +
            tr("Supported by %1").arg("agroviva GmbH");

    QString about = "<b>" + title + "</b><br/><br/>"+ descr + "<br/><br/>" + copy + "<br/><br/>" + version;

    ui.labelAbout->setText(about);
    ui.labelCredits->setText(credits);
}

AboutDlg::~AboutDlg()
{

}
