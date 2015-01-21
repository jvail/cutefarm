
#include <QSqlQuery>

#include "newharvestyeardialog.h"

NewHarvestYearDialog::NewHarvestYearDialog(int oldYear, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	m_fildUseModel = new QStandardItemModel(0, 2, this);

	QSqlDatabase::database().transaction();

	QSqlQuery q;
	q.prepare(" SELECT fild_use_id, fild_use_name "
		   " FROM fild_use WHERE fild_use_id IN"
		   " (SELECT DISTINCT fild_use_id FROM fild WHERE year_of_harvest=:oldYear) "
		   " ORDER BY fild_use_name ASC ");
	q.bindValue(":oldYear", oldYear);
	q.exec();

	QSqlDatabase::database().commit();

    int row = 0;
	while (q.next()) {
		for(int col = 0; col < 2; col++){
	    	QStandardItem *item = new QStandardItem;
	    	if(col == 0){
	    		item->setData(q.value(0) , Qt::DisplayRole);
	    	}
	    	else if(col == 1){
	    		item->setData(q.value(1), Qt::DisplayRole);
	    		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	    		item->setCheckState(Qt::Unchecked);
	    	}
	    	m_fildUseModel->setItem(row, col, item);
	   }
        row++;
	}

	ui.listView->setModel(m_fildUseModel);
	ui.listView->setModelColumn(1);
}

QSet<int> NewHarvestYearDialog::fildUseIds() const
{
	QSet<int> ids;
	for (int i = 0; i < m_fildUseModel->rowCount(); i++) {
		if (m_fildUseModel->item(i, 1)->data(Qt::CheckStateRole) == Qt::Checked)
			ids << m_fildUseModel->item(i, 0)->data(Qt::DisplayRole).toInt();
	}
	return ids;
}

NewHarvestYearDialog::~NewHarvestYearDialog()
{

}
