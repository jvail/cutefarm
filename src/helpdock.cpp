
#include <QSqlQuery>
#include <QDir>
#include <QDesktopServices>

#include <QDebug>

#include "helpdock.h"
#include "settingsdialog.h"

HelpDock::HelpDock(QWidget *parent)
    : QDockWidget(parent)
{
	ui.setupUi(this);

	QString source;
	QString language = SettingsDialog::locale();
	language.chop(3);

    QDir dir(":/hlp");
    QString filter = "help_*.html";
    QDir::Filters filters = QDir::Files;
    QDir::SortFlags sort = QDir::Name;
    QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);

    foreach (const QFileInfo &file, entries) {
        if (file.baseName().right(2) == language) {
            source = file.filePath();
            break;
        }
    }

    if (!source.isEmpty())
        ui.textBrowser->setSource(QUrl::fromLocalFile(source));
    else
        ui.textBrowser->setSource(QUrl::fromLocalFile(":/hlp/help_en.html"));

}

HelpDock::~HelpDock()
{

}

