/* 	CuteFarm
    partly from Tramp www.qt-apps.org
    Copyright (C) 2008-2011  Jan Vaillant

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation License 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include <QtGui>
#include <QDir>
#include <QApplication>
#include <QSettings>
#include <QSplashScreen>
#include <QString>
#include <QSqlDatabase>
#include <QDesktopServices>

#include "mainwindow.h"
#include "cutefarm.h"
#include "settingsdialog.h"


int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(res);

	QLocale::setDefault(QLocale::system());

	QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("CuteFarm");
//    QCoreApplication::setApplicationName("CuteFarm");
    QCoreApplication::setApplicationVersion(CuteFarm::CUTEFARM_BUILD);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    CuteFarm::createDataDir();

    a.setWindowIcon(QIcon(":/pic/ladybug256.png"));
    a.setStyle(SettingsDialog::style());

#ifdef Q_WS_MAC
    //far too small fonts on mac?
    a.setStyleSheet("QWidget { font-size: 11px; }");
#endif

	QPixmap pixmap(":/pic/ladybug256splash2.png");
	QFont f(a.font());
	f.setBold(true);
	QSplashScreen splash;
	splash.setFont(f);
	splash.setPixmap(pixmap);
	splash.setMask(pixmap.mask());
	splash.show();
	splash.showMessage("CuteFarm i18n...", Qt::AlignLeft, QColor(148, 0, 0));

//#ifdef Q_WS_MAC
//    QDir path = QDir::homePath() + "/Library/Application Support/";
//    if (!path.exists("CuteFarm"))
//        path.mkdir("CuteFarm");
//#else
//    QDir path = QDir::homePath();
//    if (!path.exists("CuteFarm"))
//        path.mkdir("CuteFarm");
//#endif

    QTranslator qtTranslator;
    QTranslator cutefarmTranslator;
    QString locale = SettingsDialog::locale();
    QString language = locale.left(2);

    qDebug() << locale << language;

    QDir dir(":/trs");
    QString filter = "cutefarm_*.qm";
    QDir::Filters filters = QDir::Files;
    QDir::SortFlags sort = QDir::Name;
    QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);

//    if (entries.isEmpty()) { //if empty check data storage location
//        dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "trs";
//        entries = dir.entryInfoList(QStringList() << filter, filters, sort);
//    }

    foreach (const QFileInfo &file, entries) {
        qDebug() << file.baseName().mid(9, 2);
        if (file.baseName().mid(9, 2) == language) {
            if (cutefarmTranslator.load("cutefarm_" + language, dir.path(), QString(), ".qm"))
                a.installTranslator(&cutefarmTranslator);
            if (qtTranslator.load("qt_" + language, dir.path()))
                a.installTranslator(&qtTranslator);
        }
    }

	splash.showMessage("CuteFarm DB...", Qt::AlignLeft, QColor(148, 0, 0));

	QSettings s("CuteFarm", "CuteFarm");
	bool success = false;

	if (!QString(argv[1]).isEmpty())
	    success = CuteFarm::connectDatabase(QString(argv[1]));
	else {
	    QString path = s.value("pathToDatabase").toString();
	    if (!QFile::exists(path))
	        success = CuteFarm::createDatabase();
	    else
	        success = CuteFarm::connectDatabase(path);
	}

	if(!success)
		return 1;

	if (language != "de" && language != "en") //only reports in German and English currently
	    language = "en";
	QDir home = QDir(CuteFarm::reportsDirPath());
	QDir res = QDir(QString(":/qry/%1_reports").arg(language));
	if (!home.exists("Examples-2"))
	    home.mkdir("Examples-2");
    foreach (const QString &file, res.entryList()) {
        QString source = QString("%1/%2").arg(res.path()).arg(file);
        QString target = QString("%1/Examples-2/%2")
                        .arg(home.path())
                        .arg(QFileInfo(file).fileName());
        if (QFile::exists(target))
            continue;
        qDebug() << QFile::copy(source, target);
        qDebug() << file << source << target;
        QFile::setPermissions(target, QFile::ReadUser | QFile::WriteUser | QFile::WriteOwner | QFile::ReadOwner);
    }

	QSettings settings;
	settings.setValue("pathToDatabase", QSqlDatabase::database().databaseName());

	a.processEvents();

	splash.showMessage("CuteFarm GUI...", Qt::AlignLeft, QColor(148, 0, 0));

	MainWindow w;
//    splash.showMessage("CuteFarm PLUGINS", Qt::AlignLeft, QColor(148, 0, 0));
//    QMetaObject::invokeMethod(&w, "loadPlugin", Qt::DirectConnection);
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    w.show();
    splash.finish(&w);

    return a.exec();
}


