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
    QState *videoresult;

    QLabel *resultDisplay;

    QLabel *earthDisplay,*moonDisplay,*marsDisplay,*jupiterDisplay;

    QTimer *displayTimer,*displayTimer2;
    QTimer *stateTimer;

    int countDownValue;
    int jumpHeight,jumpHeight2;

    double earth,moon,mars,jupiter;
    void loadCoefs();
    mpvWidget *vp;
    int earthScore,earthScore2,marsScore,marsScore2,moonScore,moonScore2,jupiterScore,jupiterScore2;
    QFont font,font2;
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

    void showVideo();
    void countDown();
    void showEarth();
    void showMoon();
    void showJupiter();
    void showMars();


protected:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // MAINSCREEN_H
