/*
 * cutefarm.cpp
 * cutefarm
 * 
 * 
 *  Created on: 17.01.2011
 *      Author: jv
 */

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDesktopServices>
#include <QApplication>
#include <QMainWindow>

#include "cutefarm.h"


CuteFarm* CuteFarm::mInstance = 0;
QString CuteFarm::CUTEFARM_BUILD = "2.1.0";
int CuteFarm::CUTEFARM_VERSION = 2;
int CuteFarm::CUTEFARM_DB_USER_VERSION = 200;
bool CuteFarm::mRunWizard = false;

CuteFarm::~CuteFarm()
{

}

CuteFarm* CuteFarm::instance()
{
    if (!mInstance)
        mInstance = new CuteFarm;

    return mInstance;
}


bool CuteFarm::createDatabase(QWidget *parent)
{
    QString path = QFileDialog::getSaveFileName(parent,
            QObject::tr("Create Database"),
            QDesktopServices::storageLocation(QDesktopServices::DataLocation),
            "SQLite3 (*.sqlite3)", 0, QFileDialog::DontConfirmOverwrite);

    if (path.isEmpty())
        return false;

    QString oldDb = QSqlDatabase::database().databaseName();
    closeDatabase();

    if (QFile::exists(path)) {
        QMessageBox::critical(0, QObject::tr("Database Error"), QObject::tr("Database exists already."));

        if (!oldDb.isEmpty())
            connectDatabase(oldDb);

        return false;
    }

    QSettings settings;
    QString locale = settings.value("locale", "").toString();
    locale.chop(3);
    if (locale.isEmpty())
        locale = QLocale::languageToString(QLocale::system().language()).toLower();

    if (!QFile::exists(QString(":/sql/%1.sqlite3").arg(locale)))
        locale ="en";

    if (!QFile::copy(QString(":/sql/%1.sqlite3").arg(locale), path)) {
        QMessageBox::critical(parent, QObject::tr("Error"), QObject::tr("Copying database file to '%1' failed.").arg(path));
        if (!oldDb.isEmpty())
            connectDatabase(oldDb);
        return false;
    }

    QFile::setPermissions(path, QFile::ReadUser | QFile::WriteUser | QFile::WriteOwner | QFile::ReadOwner);

    if (!connectDatabase(path)) {
        if (!oldDb.isEmpty())
            connectDatabase(oldDb);
        return false;
    }
    else {
        mRunWizard = true;
        return true;
    }

}

bool CuteFarm::openDatabase(QWidget *parent)
{
    QString oldPath = QFileInfo(QSqlDatabase::database().databaseName()).absolutePath();

    QString path = QFileDialog::getOpenFileName(parent,
            QObject::tr("Open Database"),
            (oldPath.isEmpty()) ? QDesktopServices::storageLocation(QDesktopServices::DataLocation) : oldPath,
            "SQLite3 (*.sqlite3)");

    if (path.isEmpty())
        return false;

    QString oldDb = QSqlDatabase::database().databaseName();
    closeDatabase();

    if(!connectDatabase(path)) {
        if (!oldDb.isEmpty())
             return connectDatabase(oldDb);
         return false;
    }

    return true;
}

bool CuteFarm::connectDatabase(const QString &path)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().driverText());
        return false;
    }
    else
        qDebug() << "DB open";

    if (!hasDatabaseUserVersion(CuteFarm::CUTEFARM_DB_USER_VERSION)) {
        closeDatabase();
        if (QMessageBox::critical(0, QObject::tr("Database Error"),
                QObject::tr("Database version is not compatible.\nCreate a new database?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            return createDatabase();
        }


        return false;
    }

    if (!hasForeignKeySupport()) {
        closeDatabase();
        QMessageBox::critical(0, QObject::tr("Database Error"),
                QObject::tr("Enabling SQLite's foreign key support failed."));

        return false;
    }

    return true;

}

void CuteFarm::closeDatabase()
{
    if (!QSqlDatabase::connectionNames().isEmpty()) {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

bool CuteFarm::hasDatabaseUserVersion(int version)
{
    QSqlQuery q;
    q.exec("PRAGMA user_version");
    qDebug() << q.lastError().text();
    return (q.first() && q.value(0).toInt() == version);
}

bool CuteFarm::hasForeignKeySupport()
{
    QSqlQuery q;
    q.exec("PRAGMA foreign_keys=1");
    q.exec("PRAGMA foreign_keys");
    qDebug() << q.lastError().text();
    return (q.first() && q.value(0).toInt() == 1);
}

void CuteFarm::createDataDir() {

    QString cuteFarmDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString reportsDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "Reports";
    QDir dir;

    if (!dir.exists(cuteFarmDir))
        dir.mkpath(cuteFarmDir);

    if (!dir.exists(reportsDir))
        dir.mkpath(reportsDir);
}

QString CuteFarm::reportsDirPath() {

    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "Reports";
}


