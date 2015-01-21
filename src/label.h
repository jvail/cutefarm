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

#ifndef LABEL_H_
#define LABEL_H_

#include <QLabel>


class Label : public QLabel {

public:
    Label(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *ev);
    void changeEvent(QEvent *event);

private:
    static void checkStyleHasFrame(QWidget *w);

    static bool mNoFrame;


};

class NutLabel : public QLabel {

public:
    NutLabel(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *ev);


};


#endif /* LABEL_H_ */
