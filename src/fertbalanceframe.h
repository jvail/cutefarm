#ifndef FERTBALANCEFRAME_H
#define FERTBALANCEFRAME_H

#include <QtGui/QFrame>

class FertBalanceFrame : public QFrame
{
    Q_OBJECT

public:
    FertBalanceFrame(QWidget *parent = 0);
    ~FertBalanceFrame();

    void setValues(double basis, double size, double value);


protected:
    void paintEvent(QPaintEvent *event);
    void changeEvent(QEvent *event);

private:
    static void checkStyleHasFrame(QWidget *w);

    double mBasis;
    double mSize;
    double mValue;

    static QColor mPositiveCol;
    static QColor mNegativeCol;

    static bool mNoFrame;
    static double mHorizontalOffset;


};

#endif // FERTBALANCEFRAME_H
