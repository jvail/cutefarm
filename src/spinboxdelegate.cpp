/* 	CuteFarm
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

#include "spinboxdelegate.h"


SpinBoxDelegate::SpinBoxDelegate(QObject *parent, double min, double max,
		bool isDouble, bool data, const QString &suffix, int dec, bool hasNULL)
    : QItemDelegate(parent)
{
	_min = min;
	_max = max;
	m_isDouble = isDouble;
	maxFromData = data;
	m_suffix = suffix;
	m_dec = dec;
	m_hasNULL = hasNULL;
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &index/* index */) const
{
	if(m_isDouble){
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		if (m_hasNULL)
			editor->setSpecialValueText("NULL");
		editor->setDecimals(m_dec);
		if(maxFromData)
			editor->setMaximum(index.data(Qt::UserRole+1).toDouble());
		else
			editor->setMaximum(_max.toDouble());
		if(!m_suffix.isEmpty())
			editor->setSuffix(m_suffix);

		editor->setMinimum(_min.toDouble());

		return editor;
	}
	else{
		QSpinBox *editor = new QSpinBox(parent);
		if(maxFromData)
			editor->setMaximum(index.data(Qt::UserRole+1).toInt());
		else
			editor->setMaximum(_max.toInt());
		if(!m_suffix.isEmpty())
			editor->setSuffix(m_suffix);

		editor->setMinimum(_min.toInt());

		return editor;
	}
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
	if(m_isDouble){
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		if (m_hasNULL) {
			QString str = index.model()->data(index, Qt::DisplayRole).toString();
			if (str == "NULL")
				spinBox->setValue(spinBox->minimum());
			else
				spinBox->setValue(index.model()->data(index, Qt::DisplayRole).toDouble());
		}
		else
			spinBox->setValue(index.model()->data(index, Qt::DisplayRole).toDouble());
	}
	else{
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
		QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(value);
	}
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
	if(m_isDouble){
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		spinBox->interpretText();
		double value = spinBox->value();
		if (m_hasNULL) {
			if (value == spinBox->minimum())
				model->setData(index, "NULL", Qt::EditRole);
			else
				model->setData(index, value, Qt::EditRole);
		}
		else
			model->setData(index, value, Qt::EditRole);
	}
	else{
		QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
		spinBox->interpretText();
		int value = spinBox->value();
	    model->setData(index, value, Qt::EditRole);
	}
}

void SpinBoxDelegate::paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	if (m_hasNULL) {
		if (index.data(Qt::DisplayRole).toString() == "NULL") {
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, QApplication::palette().brush(QPalette::Active, QPalette::Highlight));
//			else
//				QItemDelegate::paint(painter, option, index);
		}
		else
			QItemDelegate::paint(painter, option, index);
	}
	else
		QItemDelegate::paint(painter, option, index);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
