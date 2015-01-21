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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QSettings>
#include <QHeaderView>
#include <QStyleFactory>
#include <QPushButton>
#include <QMessageBox>
#include "ui_settingsdialog.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    static bool alternatingRowColors();
    static enum QHeaderView::ResizeMode resizeMode();
    static bool showGrid();
    static QString style();
    static QString locale();
    static bool lotsDisabled();
    static QString areaUnit();
    static QString nutUnit();

signals:
	void settingsChanged();

private slots:
	void restoreDefaults();
	void on_dbPathPushButton_clicked();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void updateStyle(const QString &style);

private:
	void loadTranslations();
	static QSettings mSettings;
	QStringList localeList;

    Ui::SettingsDialogClass ui;
};

#endif // SETTINGSDIALOG_H
