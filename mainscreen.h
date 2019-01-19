#ifndef MAINSCREEN_H
#define MAINSCREEN_H


#include <QWidget>
#include <qobject.h>
#include "qlabel.h"

#include "mpvwidget.h"
#include "qtimer.h"
#include "QDirIterator"
#include "qcolor.h"
#include "qcombobox.h"

#include "qmath.h"
#include "qlineedit.h"
#include "qstylefactory.h"
#include "qpropertyanimation.h"
#include "qdebug.h"
#include "qstatemachine.h"
#include "qpainter.h"

class mainScreen : public QLabel
{
    Q_OBJECT
public:
    explicit mainScreen(QLabel *parent = nullptr,QString PATH="");

private:
    QString PATH;
    void setupStates();
    QStateMachine machine;
    QState *idle;
    QState *rules;
    QState *cd;
    QState *jump;
    QState *result;

    QLabel *resultDisplay;
    QTimer *displayTimer;
    QTimer *stateTimer;

    int countDownValue;
    int jumpHeight,jumpHeight2;

    mpvWidget *vp;
signals:
    void gotJump(int);
    void gotValidJump();
    void countDownOver();
    void displayOver();

private slots:
    void getJump(int time);
    void goIdle();
    void goRules();
    void goCountDown();
    void goJump();
    void goResult();
    void updateDisplay();

    void countDown();


protected:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // MAINSCREEN_H
