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

#ifndef ACTIONSVIEWMODEL_H_
#define ACTIONSVIEWMODEL_H_

#include <QAbstractItemModel>
#include <QVector>

class ActionsViewItem;
class QTextEdit;
class QProgressDialog;
class QTextDocument;

class ActionsViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ActionsViewModel(QObject *parent);
    ~ActionsViewModel();

//    Q_ENUMS (ItemType)

    enum ItemType { Type_Fert, Type_Pltr, Type_Soil, Type_Soil_Test,
        Type_Medw, Type_Anim_Grp, Type_Harv, Type_Dril, Type_Misc, Type_NoType,
        Type_Sum, Type_Oprt, Type_Fild};

    void clearModel();
    int actnId(const QModelIndex &index) const;
    QString actnName(const QModelIndex &index) const;
    bool deleteActn(const QModelIndex &index);
    QString toHtmlRows(const QModelIndex &idx = QModelIndex()) const;
    bool select(QProgressDialog *progress = 0);
    void setFilter(const QList<ItemType> &oprtTypes, const QString &startDate, const QString &endDate,
            QProgressDialog *progress = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    ActionsViewItem *getItem(const QModelIndex &index) const;
    QString filterSql();

    ActionsViewItem *mRootItem;
    QList<ItemType> mOprtTypes;
    QString mStartDate;
    QString mEndDate;
    QTextDocument *mTextDoc;
    QString mAreaUnit;

};


class ActionsViewItem
{
public:

    ActionsViewItem(const QVector<QVariant> &data, ActionsViewItem *parent = 0);
    ~ActionsViewItem();

    int mActnId;

    QString toHtmlRow() const;
    ActionsViewItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    ActionsViewItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    ActionsViewModel::ItemType getType() const
    {
        return mType;
    }

    void setType(ActionsViewModel::ItemType mType)
    {
        this->mType = mType;
    }

    QVariant comment() const { return mComment; }
    void setComment(QVariant mComment) { this->mComment = mComment; }

private:
    QList<ActionsViewItem*> childItems;
    QVector<QVariant> itemData;
    ActionsViewItem *parentItem;
    QVariant mComment;
    ActionsViewModel::ItemType mType;
    static QTextDocument mTextDoc;


};

#endif /* ACTIONSVIEWMODEL_H_ */
