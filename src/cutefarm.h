/*
 * cutefarm.h
 *
 *  Created on: 26.03.2009
 *      Author: jv
 */

#ifndef CUTEFARMCONSTANTS_H_
#define CUTEFARMCONSTANTS_H_

#include <QObject>

//#include <cmath>

class CuteFarm : public QObject {

    Q_OBJECT

public:
    ~CuteFarm();

    static CuteFarm* instance();
    enum OprtType { TypeFert, TypePltr, TypeSoil, TypeSoilTest,
        TypeMedw, TypeAnimGrp, TypeHarv, TypeDril, TypeMisc };

    static QString CUTEFARM_BUILD;
    static  int CUTEFARM_VERSION;
    static  int CUTEFARM_DB_USER_VERSION;
    static bool mRunWizard;

    static bool createDatabase(QWidget *parent = 0);
    static bool openDatabase(QWidget *parent = 0);
    static bool connectDatabase(const QString &path);
    static void closeDatabase();
    static bool hasDatabaseUserVersion(int version);
    static bool hasForeignKeySupport();
    static void createDataDir();
    static QString reportsDirPath();


private:
    CuteFarm(QObject *parent = 0) : QObject(parent) {};

    CuteFarm(const CuteFarm &); // hide copy constructor
    CuteFarm& operator=(const CuteFarm &); // hide assign op
                                 // we leave just the declarations, so the compiler will warn us
                                 // if we try to use those two functions by accident

    static CuteFarm* mInstance;

};


#endif /* CUTEFARMCONSTANTS_H_ */
