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

#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

#include "label.h"

bool Label::mNoFrame = false;

Label::Label(QWidget *parent)
    : QLabel(parent)
{
    QLabel::setMinimumHeight(20);

    checkStyleHasFrame(this);
}

void Label::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);
    QPainter painter(this);
    QFontMetrics fm(font());

    painter.drawText(ev->rect().adjusted(4, 1, -1, -1), fm.elidedText(text(), Qt::ElideRight, width()), QLabel::alignment());
//    QPen pen;
//    pen.setColor(palette().color(QPalette::Mid));
//    painter.setPen(pen);
//    painter.drawRoundedRect(ev->rect().adjusted(0, 0, -1, -1), 2.5, 2.5);

    QPen pen;
    pen.setColor(palette().color(QPalette::Mid));
    painter.setPen(pen);

    //No frames in gtk+ and oxygen: sux
    if (mNoFrame) {
        QStyleOptionFrameV3 option;
        option.initFrom(this);
        option.state = QStyle::State_Sunken;
        option.frameShape = QFrame::HLine;
        style()->drawPrimitive(QStyle::PE_Frame, &option, &painter, this);
//        QPen pen;
//        pen.setColor(palette().color(QPalette::Mid));
//        painter.setPen(pen);
//        painter.drawRoundedRect(ev->rect().adjusted(0, 0, -1, -1), 3.1, 3.1);
    }
    else {
        QStyleOptionFrameV3 option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_Frame, &option, &painter, this);
    }
}

void Label::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        checkStyleHasFrame(this);

    QFrame::changeEvent(event);
}

void Label::checkStyleHasFrame(QWidget *w)
{
    QString style(w->style()->metaObject()->className());
    mNoFrame = (style.contains("oxygen", Qt::CaseInsensitive)
            || style.contains("gtk", Qt::CaseInsensitive));
}


NutLabel::NutLabel(QWidget *parent)
    : QLabel(parent)
{
    QLabel::setFixedHeight(18);
    QLabel::setFixedWidth(37);
    QLabel::setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void NutLabel::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);
    QPainter painter(this);
    QFontMetrics fm(font());

    painter.drawText(ev->rect().adjusted(2, 1, -1, -1), fm.elidedText(text(), Qt::ElideRight, width()), QLabel::alignment());

//    QPen pen;
//    pen.setColor(palette().color(QPalette::Mid));
//    painter.setPen(pen);
//    painter.drawRoundedRect(ev->rect().adjusted(0, 0, -1, -1), 2, 2);
}

