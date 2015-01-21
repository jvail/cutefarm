
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include <QDebug>

#include "fertbalanceframe.h"

//TODO: craete new baseclass label/FertBalanceFrame

QColor FertBalanceFrame::mPositiveCol = QColor("#53801b");
QColor FertBalanceFrame::mNegativeCol = QColor("#006aa9");
bool FertBalanceFrame::mNoFrame = false;
double FertBalanceFrame::mHorizontalOffset = 1.;

FertBalanceFrame::FertBalanceFrame(QWidget *parent)
    : QFrame(parent)
{
    mBasis = 0;
    mSize = 0;
    mValue = 0;

    setMinimumHeight(40);
    QFrame::setFixedWidth(37);
    setFrameShape(QFrame::NoFrame);

    checkStyleHasFrame(this);
}

FertBalanceFrame::~FertBalanceFrame()
{

}

void FertBalanceFrame::setValues(double basis, double size, double value)
{
    mBasis = basis;
    mSize = size;
    mValue = value;

    update();
}

void FertBalanceFrame::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    double offset = 10.;
    double h = height()-offset;

    if (mSize == 0) {
        mSize = width();
        mBasis = double(width())/2.0;
        mValue = mBasis;
    }

    QPainter painter(this);

    int yBase = h-((mBasis/mSize)*h);
    int yValue = h-((mValue/mSize)*h);

    painter.fillRect(mHorizontalOffset, yValue > yBase ? yBase+offset/2 : yValue+offset/2, event->rect().width()-(mHorizontalOffset*2),
            yValue > yBase ? yValue-yBase : yBase-yValue, yValue > yBase ? mNegativeCol : mPositiveCol);

    QPen pen;
    pen.setColor(palette().color(QPalette::Mid));
//    pen.setColor(QColor("#ffb92d"));
//    pen.setWidthF(3);
    painter.save();
    painter.setPen(pen);
//    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.drawLine(mHorizontalOffset, yBase+offset/2., event->rect().width()-2, yBase+offset/2.);
//    painter.drawRoundedRect(event->rect().adjusted(0, 0, -1, -1), 3.1, 3.1);
    painter.restore();

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
//        painter.drawRoundedRect(event->rect().adjusted(0, 0, -1, -1), 3.1, 3.1);
    }
    else {
        QStyleOptionFrameV3 option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_Frame, &option, &painter, this);
    }
}

void FertBalanceFrame::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::StyleChange)
        checkStyleHasFrame(this);

    QFrame::changeEvent(event);
}

void FertBalanceFrame::checkStyleHasFrame(QWidget *w)
{
    QString style(w->style()->metaObject()->className());
    mNoFrame = (style.contains("oxygen", Qt::CaseInsensitive)
            || style.contains("gtk", Qt::CaseInsensitive));

    mHorizontalOffset = (mNoFrame) ? 2 : 1;
}
