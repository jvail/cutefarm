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

#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_


#include <QProgressBar>


class ProgressBar : public QProgressBar {

public:
    ProgressBar(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *ev);

private:
    QPixmap mLady;


};


#endif /* PROGRESSBAR_H_ */
