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

#ifndef TEXTINPUTDIALOG_H
#define TEXTINPUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_textinputdialog.h"

class TextInputDialog : public QDialog
{
    Q_OBJECT

public:
    TextInputDialog(const QString &label, QIcon icon
    		, const QString &title, QWidget *parent = 0);
    ~TextInputDialog();
    
    QString text() const;

private:
    Ui::TextInputDialogClass ui;
};

#endif // TEXTINPUTDIALOG_H
