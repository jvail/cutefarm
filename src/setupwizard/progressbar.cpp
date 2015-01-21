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

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QDebug>

#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    mLady = QPixmap(":/pic/ladybug30x22_frw");
}

void ProgressBar::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)

    QPainter painter(this);
    int x = (double(value())/double(maximum()))*double(width()-22.);
    painter.drawPixmap(x, (height()-18)/2, mLady);

    QStyleOptionFrameV3 option;
    option.initFrom(this);
    option.state = option.state | QStyle::State_HasFocus;
    style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, &painter, this);

    QFont f(font());
    painter.setFont(f);
    painter.drawText(ev->rect(), text(), Qt::AlignHCenter | Qt::AlignVCenter);
}
