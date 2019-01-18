#include "mainscreen.h"

mainScreen::mainScreen(QLabel *parent,QString PATH) : QLabel(parent),PATH(PATH)
{
    resize(1080,1920);

    displayTimer = new QTimer(this);
    displayTimer->setInterval(50);
    connect(displayTimer,SIGNAL(timeout()),this,SLOT(updateDisplay()));

    resultDisplay = new QLabel(this);
    resultDisplay->resize(380,200);
    resultDisplay->move((width()-resultDisplay->width())/2,(height()-resultDisplay->height())/2);
    //resultDisplay->setStyleSheet("border: 2px solid white");
    //resultDisplay->show();

    jumpHeight = 66;
    goResult();
    //setupStates();


}



void mainScreen::setupStates()
{
    idle = new QState();
    rules = new QState();
    cd = new QState();
    jump = new QState();
    result = new QState();


    connect(idle,SIGNAL(entered()),this,SLOT(goIdle()));

    idle->addTransition(this,SIGNAL(gotJump(int)),rules);
    connect(rules,SIGNAL(entered()),this,SLOT(goRules()));


    rules->addTransition(this,SIGNAL(countDownOver()),cd);
    connect(cd,SIGNAL(entered()),this,SLOT(goCountDown()));

    cd->addTransition(this,SIGNAL(countDownOver()),jump);
    connect(jump,SIGNAL(entered()),this,SLOT(goJump()));

    jump->addTransition(this,SIGNAL(gotValidJump()),result);
    connect(result,SIGNAL(entered()),this,SLOT(goResult()));


    machine.addState(idle);
    machine.addState(rules);
    machine.addState(cd);
    machine.addState(jump);
    machine.addState(result);
    machine.setInitialState(idle);
    machine.start();
}



void mainScreen::goIdle()
{
    resultDisplay->hide();
    jumpHeight=0;
    setPixmap(QPixmap(PATH+"idle.png").scaled(width(),height()));
}

void mainScreen::goRules()
{
    setPixmap(QPixmap(PATH+"rules.png").scaled(width(),height()));
    QTimer::singleShot(4000,this,SIGNAL(countDownOver()));

}


void mainScreen::goCountDown()
{
    countDownValue = 3;
    countDown();
}


void mainScreen::countDown()
{
    if(countDownValue<=0)
    {
        emit countDownOver();
        return;
    }

    setPixmap(QPixmap(PATH+QString::number(countDownValue)+".png").scaled(width(),height()));
    countDownValue--;

    QTimer::singleShot(1000,this,SLOT(countDown()));

}


void mainScreen::goJump()
{
    setPixmap(QPixmap(PATH+"jump.png").scaled(width(),height()));
}


void mainScreen::goResult()
{
    setPixmap(QPixmap(PATH+"result.png").scaled(width(),height()));

    jumpHeight2 = 0;

    resultDisplay->show();

    displayTimer->start();
}


void mainScreen::updateDisplay()
{
    jumpHeight2+=1;


    if(jumpHeight2>=jumpHeight)
    {
        jumpHeight2 = jumpHeight;
        displayTimer->stop();
        emit displayOver();
    }

    QString text = QString("%1").arg(jumpHeight2, 3, 10, QChar('0'));

    QPixmap pix(resultDisplay->size());
    pix.fill(Qt::transparent);

    QPainter painter(&pix);




    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 0));

    QFont font = QFont(PATH+"font.ttf",110);
    painter.setFont(font);

    QFontMetrics fm(font);

    double W = fm.width("100");
    double H = fm.height();


    painter.drawText(QRectF((resultDisplay->width()-W)/2,(resultDisplay->height()-H)/2-20,width(),height()),text);

    font = QFont(PATH+"font.ttf",50);
    painter.setFont(font);
    W = QFontMetrics(font).width("cm");
    painter.drawText(QRectF((resultDisplay->width()-W)/2,(resultDisplay->height()-H)/2+110,width(),height()),"cm");



    resultDisplay->setPixmap(pix);


}

void mainScreen::getJump(int time)
{
    if((machine.configuration().contains(jump))&&(time>30))
    {
        double h = (double)time*115/1000;

        jumpHeight = h;
        emit gotValidJump();
    }
}



void mainScreen::mousePressEvent(QMouseEvent *ev)
{
    qDebug()<<ev->pos();

}
