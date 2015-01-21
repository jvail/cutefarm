/*  CuteFarm
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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QTextDocument>
#include <QPixmap>
#include <QProgressDialog>
#include <QDate>

#include <QDebug>

#include "actionsviewmodel.h"
#include "settingsdialog.h"


ActionsViewModel::ActionsViewModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    mTextDoc = new QTextDocument;


    QVector<QVariant> head;
    head << tr("Action") << tr("Detail") << tr("Comment");

    mRootItem = new ActionsViewItem(head);

    mStartDate = QDate::currentDate().addDays(-7).toString("yyyy-MM-dd");
    mEndDate = QDate::currentDate().toString("yyyy-MM-dd");

    mAreaUnit = SettingsDialog::areaUnit();

}

ActionsViewModel::~ActionsViewModel()
{
    delete mRootItem;
    delete mTextDoc;
}

void ActionsViewModel::clearModel()
{
    QAbstractItemModel::beginResetModel();
    mRootItem->removeChildren(0, mRootItem->childCount());
    QAbstractItemModel::reset();
    QAbstractItemModel::endResetModel();
}

int ActionsViewModel::actnId(const QModelIndex &index) const
{
    if (!index.isValid())
        return -1;

    return getItem(index)->mActnId;
}

QString ActionsViewModel::actnName(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    ActionsViewItem *item = getItem(index);

    if (item->mActnId < 0)
        return QString();

    while (item->parent() != mRootItem)
        item = item->parent();

    return QString("%1 / %2").arg(item->data(0).toString()).arg(item->data(1).toString());
}

bool ActionsViewModel::deleteActn(const QModelIndex &index)
{
    ActionsViewItem *item = getItem(index);

    if (item->mActnId < 0)
        return false;

    while (item->parent() != mRootItem)
        item = item->parent();

    if (item->getType() == Type_Oprt) {
        beginRemoveRows(QModelIndex(), item->childNumber(), item->childNumber());
        mRootItem->removeChildren(item->childNumber(), 1);
        endRemoveRows();
        return true;
    }

    return false;
}

QString ActionsViewModel::toHtmlRows(const QModelIndex &idx) const
{
    QString rows;

    if (!idx.isValid()) {
        for (int row = 0; row < mRootItem->childCount(); ++row)
            rows.append(toHtmlRows(index(row, 0)));
    }
    else {
        rows.append(getItem(idx)->toHtmlRow());
        for (int row = 0; row < getItem(idx)->childCount(); ++row)
            rows.append(toHtmlRows(index(row, 0, idx)));
    }

    return rows;
}

bool ActionsViewModel::select(QProgressDialog *progress)
{
    QSqlDatabase::database().transaction();

    QAbstractItemModel::beginResetModel();

    mRootItem->removeChildren(0, mRootItem->childCount());

    QLocale locale;

    QSqlQuery qCount;
    if (!mOprtTypes.isEmpty() && progress) {
        qCount.prepare("SELECT count(*) FROM actn "
                " WHERE date(actn_date) >= :startDate AND date(actn_date) <= :endDate "
                + filterSql() +
                " LIMIT 1000");
        qCount.bindValue(":startDate", mStartDate);
        qCount.bindValue(":endDate", mEndDate);
        qCount.exec();

        while (qCount.next())
            progress->setMaximum(qCount.value(0).toInt());

    }


    QSqlQuery qActn;
    QSqlQuery qField;
    if (!mOprtTypes.isEmpty()) {
        qActn.prepare("SELECT actn_id, oprt_name || ' / ' || work_name, actn_date, "
                " CASE WHEN trim(actn_com)='' THEN NULL ELSE actn_com END AS actn_com "
                " FROM actn, oprt, work "
                " WHERE actn.oprt_id=oprt.oprt_id "
                " AND actn.work_id=work.work_id "
                " AND date(actn_date) >= :startDate AND date(actn_date) <= :endDate "
                + filterSql() +
                " ORDER BY date(actn_date) DESC "
                " LIMIT 1000");
        qActn.bindValue(":startDate", mStartDate);
        qActn.bindValue(":endDate", mEndDate);

        qField.prepare("SELECT fild_id, fild_name, area,"
                " CASE WHEN trim(fild_com)='' THEN NULL ELSE fild_com END AS fild_com,"
                " year_of_harvest "
                " FROM actn_fild "
                " JOIN fild USING(fild_id) "
                " WHERE actn_id=:actn_id");
    }

    QSqlQuery qFert;
    if (mOprtTypes.contains(Type_Fert))
        qFert.prepare("SELECT fert_id, fert_name, actn_fert_amt,"
                " CASE WHEN trim(fert_unit)='' THEN '?' ELSE fert_unit END AS fert_unit "
                " FROM actn_fert "
                " JOIN fert USING(fert_id) "
                " WHERE actn_id=:actn_id "
                " ORDER BY fert_name ");
    QSqlQuery qPltr;
    if (mOprtTypes.contains(Type_Pltr))
        qPltr.prepare("SELECT pltr_id, pltr_name, actn_pltr_amt,"
                " CASE WHEN trim(pltr_unit)='' THEN '?' ELSE pltr_unit END AS pltr_unit"
                " FROM actn_pltr "
                " JOIN pltr USING(pltr_id) "
                " WHERE actn_id=:actn_id "
                " ORDER BY pltr_name ");
    QSqlQuery qDril;
    if (mOprtTypes.contains(Type_Dril))
        qDril.prepare("SELECT seed_id, a.cultivar_name, actn_dril_amt, seed_unit FROM actn_dril "
                " JOIN "
                " ( SELECT seed_id, cultivar_name, seed_unit FROM seed "
                " JOIN cultivar USING(cultivar_id) ) AS a "
                " USING (seed_id) "
                " WHERE actn_id=:actn_id "
                " ORDER BY a.cultivar_name ");
    QSqlQuery qHarv;
    if (mOprtTypes.contains(Type_Harv))
        qHarv.prepare("SELECT crop_use_id, crop_use_name, actn_harv_amt,"
                " CASE WHEN trim(crop_use_harv_unit)='' THEN '?' ELSE crop_use_harv_unit END AS crop_use_harv_unit "
                " FROM actn_harv "
                " JOIN crop_use USING(crop_use_id) "
                " WHERE actn_id=:actn_id "
                " ORDER BY crop_use_name ");
    QSqlQuery qSoil;
    if (mOprtTypes.contains(Type_Soil))
        qSoil.prepare("SELECT actn_soil_id, actn_soil_name FROM actn_soil "
                " WHERE actn_id=:actn_id");
    QSqlQuery qMedw;
    if (mOprtTypes.contains(Type_Medw))
    qMedw.prepare("SELECT actn_medw_id, actn_medw_name FROM actn_medw "
                " WHERE actn_id=:actn_id");
    QSqlQuery qMisc;
    if (mOprtTypes.contains(Type_Misc))
        qMisc.prepare("SELECT actn_misc_id, actn_mics_name FROM actn_misc "
                " WHERE actn_id=:actn_id");
    QSqlQuery qAnimGrp;
    if (mOprtTypes.contains(Type_Anim_Grp))
        qAnimGrp.prepare("SELECT anim_grp_id, anim_grp_name, actn_anim_grp_amt FROM actn_anim_grp "
                " JOIN anim_grp USING(anim_grp_id) "
                " WHERE actn_id=:actn_id "
                " ORDER BY anim_grp_name ");
    QSqlQuery qSoilTest;
    if (mOprtTypes.contains(Type_Soil_Test))
        qSoilTest.prepare("SELECT actn_soil_test_id, actn_soil_test_name FROM actn_soil_test "
                " WHERE actn_id=:actn_id");

    qActn.exec();
//    qDebug() << qActn.lastQuery();
    QList<ActionsViewItem *> actns;
    while (qActn.next()) {

        if (progress) {
            if (progress->wasCanceled())
                break;

            progress->setValue(qActn.at());
        }

        mRootItem->insertChildren(mRootItem->childCount(), 1, 4);
        ActionsViewItem *actn = mRootItem->child(mRootItem->childCount()-1);
        actn->setData(0, qActn.value(1));
        actn->setData(1, qActn.value(2).toDate().toString(Qt::DefaultLocaleShortDate));
        actn->setData(2, qActn.value(3));
        mTextDoc->setHtml(qActn.value(3).toString());
        actn->setData(3, mTextDoc->toPlainText().simplified()); //Comment Plain Text
        actn->setType(Type_Oprt);
        actn->mActnId = qActn.value(0).toInt();

        QMap<int, double> fertSums;
        QMap<int, QVariant> fertNames;
        QMap<int, QVariant> fertUnits;

        QMap<int, double> pltrSums;
        QMap<int, QVariant> pltrNames;
        QMap<int, QVariant> pltrUnits;

        QMap<int, double> drilSums;
        QMap<int, QVariant> drilNames;
        QMap<int, QVariant> drilUnits;

        QMap<int, double> harvSums;
        QMap<int, QVariant> harvNames;
        QMap<int, QVariant> harvUnits;


        qField.bindValue(":actn_id", qActn.value(0));
        qField.exec();
        int harvestYear = 0;
//        QList<ActionsViewItem *> fields;
        while (qField.next()) {

            actn->insertChildren(actn->childCount(), 1, 4);
            ActionsViewItem *field = actn->child(actn->childCount()-1);
            field->setData(0, qField.value(1));
            field->setData(1, tr("%1 %2 worked").arg(locale.toString(qField.value(2).toDouble(),'f', 3)).arg(mAreaUnit));
            field->setData(2, qField.value(3));
            mTextDoc->setHtml(qField.value(3).toString());
            field->setData(3, mTextDoc->toPlainText().simplified()); //Comment Plain Text
            field->setType(Type_Fild);
            field->mActnId = qActn.value(0).toInt();
            harvestYear = qField.value(4).toInt();

            //fert
            qFert.bindValue(":actn_id", qActn.value(0));
            qFert.exec();
            while (qFert.next()) {

                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *fert = field->child(field->childCount()-1);
                fert->setData(0, qFert.value(1));
                fert->setData(1, QString("%1 %2/%5 - %3 %2/%4")
                        .arg(locale.toString(qFert.value(2).toDouble(), 'f', 2))
                        .arg(qFert.value(3).toString())
                        .arg(locale.toString(qFert.value(2).toDouble()*qField.value(2).toDouble(), 'f', 2))
                        .arg(tr("field"))
                        .arg(mAreaUnit));
                fert->setType(Type_Fert);
                fert->mActnId = qActn.value(0).toInt();

                fertSums.insert(qFert.value(0).toInt(),
                        fertSums.value(qFert.value(0).toInt(), 0)+(qFert.value(2).toDouble()*qField.value(2).toDouble()));
                fertNames.insert(qFert.value(0).toInt(), qFert.value(1));
                fertUnits.insert(qFert.value(0).toInt(), qFert.value(3));
            }

            //pltr
            qPltr.bindValue(":actn_id", qActn.value(0));
            qPltr.exec();
            while (qPltr.next()) {

                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *pltr = field->child(field->childCount()-1);
                pltr->setData(0, qPltr.value(1));
                pltr->setData(1, QString("%1 %2/%5 - %3 %2/%4")
                        .arg(locale.toString(qPltr.value(2).toDouble(), 'f', 2))
                        .arg(qPltr.value(3).toString())
                        .arg(locale.toString(qPltr.value(2).toDouble()*qField.value(2).toDouble(), 'f', 2))
                        .arg(tr("field"))
                        .arg(mAreaUnit));
                pltr->setType(Type_Pltr);
                pltr->mActnId = qActn.value(0).toInt();

                pltrSums.insert(qPltr.value(0).toInt(),
                        pltrSums.value(qPltr.value(0).toInt(), 0)+(qPltr.value(2).toDouble()*qField.value(2).toDouble()));
                pltrNames.insert(qPltr.value(0).toInt(), qPltr.value(1));
                pltrUnits.insert(qPltr.value(0).toInt(), qPltr.value(3));
            }

            //dril
            qDril.bindValue(":actn_id", qActn.value(0));
            qDril.exec();
            while (qDril.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *dril = field->child(field->childCount()-1);
                dril->setData(0, qDril.value(1));
                dril->setData(1, QString("%1 %2/%5 - %3 %2/%4")
                        .arg(locale.toString(qDril.value(2).toDouble(), 'f', 2))
                        .arg(qDril.value(3).toString())
                        .arg(locale.toString(qDril.value(2).toDouble()*qField.value(2).toDouble(), 'f', 2))
                        .arg(tr("field"))
                        .arg(mAreaUnit));
                dril->setType(Type_Dril);
                dril->mActnId = qActn.value(0).toInt();

                drilSums.insert(qDril.value(0).toInt(),
                        drilSums.value(qDril.value(0).toInt(), 0)+(qDril.value(2).toDouble()*qField.value(2).toDouble()));
                drilNames.insert(qDril.value(0).toInt(), qDril.value(1));
                drilUnits.insert(qDril.value(0).toInt(), qDril.value(3));
            }

            //harv
            qHarv.bindValue(":actn_id", qActn.value(0));
            qHarv.exec();
            while (qHarv.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *harv = field->child(field->childCount()-1);
                harv->setData(0, qHarv.value(1));
                harv->setData(1, QString("%1 %2/%5 - %3 %2/%4")
                        .arg(locale.toString(qHarv.value(2).toDouble(), 'f', 2))
                        .arg(qHarv.value(3).toString())
                        .arg(locale.toString(qHarv.value(2).toDouble()*qField.value(2).toDouble(), 'f', 2))
                        .arg(tr("field"))
                        .arg(mAreaUnit));
                harv->setType(Type_Harv);
                harv->mActnId = qActn.value(0).toInt();

                harvSums.insert(qHarv.value(0).toInt(),
                        harvSums.value(qHarv.value(0).toInt(), 0)+(qHarv.value(2).toDouble()*qField.value(2).toDouble()));
                harvNames.insert(qHarv.value(0).toInt(), qHarv.value(1));
                harvUnits.insert(qHarv.value(0).toInt(), qHarv.value(3));
            }

            //soil
            qSoil.bindValue(":actn_id", qActn.value(0));
            qSoil.exec();
            while (qSoil.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *soil = field->child(field->childCount()-1);
                soil->setData(0, qSoil.value(1));
                soil->setType(Type_Soil);
                soil->mActnId = qActn.value(0).toInt();
            }

            //medw
            qMedw.bindValue(":actn_id", qActn.value(0));
            qMedw.exec();
            while (qMedw.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *medw = field->child(field->childCount()-1);
                medw->setData(0, qMedw.value(1));
                medw->setType(Type_Medw);
                medw->mActnId = qActn.value(0).toInt();
            }

            //misc
            qMisc.bindValue(":actn_id", qActn.value(0));
            qMisc.exec();
            while (qMisc.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *misc = field->child(field->childCount()-1);
                misc->setData(0, qMisc.value(1));
                misc->setType(Type_Misc);
                misc->mActnId = qActn.value(0).toInt();
            }

            //anim
            qAnimGrp.bindValue(":actn_id", qActn.value(0));
            qAnimGrp.exec();
            while (qAnimGrp.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *animGrp = field->child(field->childCount()-1);
                animGrp->setData(0, qAnimGrp.value(1));
                animGrp->setData(1, tr("%1 animals")
                        .arg(qAnimGrp.value(2).toInt()));
                animGrp->setType(Type_Anim_Grp);
                animGrp->mActnId = qActn.value(0).toInt();
            }

            //soiltest
            qSoilTest.bindValue(":actn_id", qActn.value(0));
            qSoilTest.exec();
            while (qSoilTest.next()) {
                field->insertChildren(field->childCount(), 1, 4);
                ActionsViewItem *soilTest = field->child(field->childCount()-1);
                soilTest->setData(0, qSoilTest.value(1));
                soilTest->setType(Type_Soil_Test);
                soilTest->mActnId = qActn.value(0).toInt();
            }



        }

        //add harvestYear to actn
        QString harvYearAndDate = tr("%1 (Harvest %2)")
                .arg(actn->data(1).toString())
                .arg(harvestYear);
        actn->setData(1, harvYearAndDate);

        //sums
        actn->insertChildren(actn->childCount(), 1, 4);
        ActionsViewItem *sum = actn->child(actn->childCount()-1);
        sum->setType(Type_Sum);
        sum->setData(0, "\u2211");
        sum->mActnId = qActn.value(0).toInt();

        QMapIterator<int, double> iFertSums(fertSums);
        while (iFertSums.hasNext()) {
            iFertSums.next();
            sum->insertChildren(sum->childCount(), 1, 4);
            ActionsViewItem *sumFert = sum->child(sum->childCount()-1);
            sumFert->setData(0, QString("\u2211 %1").arg(fertNames.value(iFertSums.key()).toString()));
            sumFert->setData(1, QString("%1 %2")
                .arg(locale.toString(iFertSums.value(), 'f', 2))
                .arg(fertUnits.value(iFertSums.key()).toString()));
            sumFert->mActnId = qActn.value(0).toInt();
        }

        QMapIterator<int, double> iPltrSums(pltrSums);
        while (iPltrSums.hasNext()) {
            iPltrSums.next();
            sum->insertChildren(sum->childCount(), 1, 4);
            ActionsViewItem *sumPltr = sum->child(sum->childCount()-1);
            sumPltr->setData(0, QString("\u2211 %1").arg(pltrNames.value(iPltrSums.key()).toString()));
            sumPltr->setData(1, QString("%1 %2")
                .arg(locale.toString(iPltrSums.value(), 'f', 2))
                .arg(pltrUnits.value(iPltrSums.key()).toString()));
            sumPltr->mActnId = qActn.value(0).toInt();
        }

        QMapIterator<int, double> iDrilSums(drilSums);
        while (iDrilSums.hasNext()) {
            iDrilSums.next();
            sum->insertChildren(sum->childCount(), 1, 4);
            ActionsViewItem *sumDril = sum->child(sum->childCount()-1);
            sumDril->setData(0, QString("\u2211 %1").arg(drilNames.value(iDrilSums.key()).toString()));
            sumDril->setData(1, QString("%1 %2")
                .arg(locale.toString(iDrilSums.value(), 'f', 2))
                .arg(drilUnits.value(iDrilSums.key()).toString()));
            sumDril->mActnId = qActn.value(0).toInt();
        }

        QMapIterator<int, double> iHarvSums(harvSums);
        while (iHarvSums.hasNext()) {
            iHarvSums.next();
            sum->insertChildren(sum->childCount(), 1, 4);
            ActionsViewItem *sumHarv = sum->child(sum->childCount()-1);
            sumHarv->setData(0, QString("\u2211 %1").arg(harvNames.value(iHarvSums.key()).toString()));
            sumHarv->setData(1, QString("%1 %2")
                .arg(locale.toString(iHarvSums.value(), 'f', 2))
                .arg(harvUnits.value(iHarvSums.key()).toString()));
            sumHarv->mActnId = qActn.value(0).toInt();
        }

        if (sum->childCount() == 0)
            actn->removeChildren(sum->childNumber(), 1);
    }

    QAbstractItemModel::reset();
    QAbstractItemModel::endResetModel();

    QSqlDatabase::database().commit();

    return true;
}

void ActionsViewModel::setFilter(const QList<ItemType> &oprtTypes, const QString &startDate, const QString &endDate,
        QProgressDialog *progress)
{
    mOprtTypes = oprtTypes;
    mStartDate = startDate;
    mEndDate = endDate;

    qDebug() << oprtTypes;
    qDebug() << startDate;
    qDebug() << endDate;

    select(progress);
}

QVariant ActionsViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == 2) {
            return getItem(index)->data(3);
        }

        return getItem(index)->data(index.column());
    }

    if (role == Qt::ToolTipRole || role == Qt::DecorationRole) {
        ActionsViewItem *item = getItem(index);
        if (index.column() == 2 && !item->data(index.column()).isNull()) {
            if (role == Qt::ToolTipRole)
                return item->data(index.column());
            else
                return QPixmap(":/pic/comment16.png");
        }
        else if (index.column() == 0) {

            switch (item->getType()) {
                case Type_Fert:
                    return QPixmap(":/pic/fert16.png");
                case Type_Pltr:
                    return QPixmap(":/pic/pltr16.png");
                case Type_Anim_Grp:
                    return QPixmap(":/pic/cow16.png");
                case Type_Dril:
                    return QPixmap(":/pic/crop16.png");
                case Type_Harv:
                    return QPixmap(":/pic/cut16.png");
                case Type_Medw:
                    return QPixmap(":/pic/meadow16.png");
                case Type_Misc:
                    return QPixmap(":/pic/comment16.png");
                case Type_Soil:
                    return QPixmap(":/pic/soilfield16.png");
                case Type_Soil_Test:
                    return QPixmap(":/pic/soiltesting16.png");
                default:
                    return QVariant();
            }
        }

    }


    return QVariant();
}

QVariant ActionsViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        return mRootItem->data(section);

    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex ActionsViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    ActionsViewItem *parentItem = getItem(parent);

    ActionsViewItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

Qt::ItemFlags ActionsViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex ActionsViewModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ActionsViewItem *childItem = getItem(index);
    ActionsViewItem *parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool ActionsViewModel::hasChildren ( const QModelIndex & parent) const
{
    ActionsViewItem *item = getItem(parent);

    return (item->childCount() != 0);
}

int ActionsViewModel::rowCount(const QModelIndex &parent) const
{
    ActionsViewItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

int ActionsViewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return mRootItem->columnCount();
}

ActionsViewItem *ActionsViewModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ActionsViewItem *item = static_cast<ActionsViewItem*>(index.internalPointer());
        if (item) return item;
    }
    return mRootItem;
}

QString ActionsViewModel::filterSql()
{
    QString sql = " AND (";

    foreach (ItemType type, mOprtTypes) {
        switch (type) {
            case Type_Fert:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_fert) OR ");
                break;
            case Type_Pltr:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_pltr) OR ");
                break;
            case Type_Soil:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_soil) OR ");
                break;
            case Type_Soil_Test:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_soil_test) OR ");
                break;
            case Type_Medw:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_medw) OR ");
                break;
            case Type_Anim_Grp:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_anim_grp) OR ");
                break;
            case Type_Harv:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_harv) OR ");
                break;
            case Type_Dril:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_dril) OR ");
                break;
            case Type_Misc:
                sql.append("actn.actn_id IN (SELECT actn_id FROM actn_misc) OR ");
                break;
            default:
                break;
        }
    }

    if (mOprtTypes.isEmpty())
        return QString();

    sql.chop(3);
    sql.append(" ) ");

    return sql;
}


QTextDocument ActionsViewItem::mTextDoc;

ActionsViewItem::ActionsViewItem(const QVector<QVariant> &data, ActionsViewItem *parent)
{
    mActnId = -1;
    parentItem = parent;
    itemData = data;
    mType = ActionsViewModel::Type_NoType;
}

ActionsViewItem::~ActionsViewItem()
{
    qDeleteAll(childItems);
}

QString ActionsViewItem::toHtmlRow() const
{
    QString c;
    QString tab;
    QString comt;

    switch (mType) {
        case ActionsViewModel::Type_Oprt:
            c = "class=\"oprt\"";
            comt = "class=\"oprt_comt\"";
            break;
        case ActionsViewModel::Type_Fild:
            c = "class=\"fild\"";
            tab = "&nbsp;&nbsp;";
            comt = "class=\"fild_comt\"";
            break;
        case ActionsViewModel::Type_Sum:
            c = "class=\"sum\"";
            tab = "&nbsp;&nbsp;";
            comt = "class=\"fild_comt\"";
            break;
        default:
            c = "class=\"row\"";
            tab = "&nbsp;&nbsp;&nbsp;&nbsp;";
            break;
    }

    QString tr = QString("<tr %1>").arg(c);
    for (int i = 0; i < 3; ++i) {//without plaintext comment comments (col 3)
        if (i == 2) {
            mTextDoc.setHtml(itemData.at(i).toString());
            tr.append(QString("<td %1><p %1>").arg(comt)).append(Qt::escape(mTextDoc.toPlainText())).append("</p></td>");
        }
        else if (i == 0)
            tr.append(QString("<td %1>%2").arg(c).arg(tab)).append(Qt::escape(itemData.at(i).toString())).append("</td>");
        else
            tr.append(QString("<td %1>").arg(c)).append(Qt::escape(itemData.at(i).toString())).append("</td>");

    }

    tr.append("</tr>\n");

    return tr;
}

ActionsViewItem *ActionsViewItem::child(int number)
{
    return childItems.value(number);
}

int ActionsViewItem::childCount() const
{
    return childItems.count();
}

int ActionsViewItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<ActionsViewItem*>(this));

    return 0;
}

int ActionsViewItem::columnCount() const
{
    return 3;
}

QVariant ActionsViewItem::data(int column) const
{
    return itemData.value(column);
}

bool ActionsViewItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        ActionsViewItem *item = new ActionsViewItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool ActionsViewItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (ActionsViewItem *child, childItems)
        child->insertColumns(position, columns);

    return true;
}

ActionsViewItem *ActionsViewItem::parent()
{
    return parentItem;
}

bool ActionsViewItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool ActionsViewItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (ActionsViewItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

bool ActionsViewItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}


