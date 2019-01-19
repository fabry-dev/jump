#include "mainscreen.h"

mainScreen::mainScreen(QLabel *parent,QString PATH) : QLabel(parent),PATH(PATH)
{
   // showFullScreen();
    resize(1080,1920);
    loadCoefs();

    displayTimer = new QTimer(this);
    displayTimer->setInterval(75);
    connect(displayTimer,SIGNAL(timeout()),this,SLOT(updateDisplay()));



    resultDisplay = new QLabel(this);
    resultDisplay->resize(380,200);
    resultDisplay->move((width()-resultDisplay->width())/2,(height()-resultDisplay->height())/2);
    //resultDisplay->setStyleSheet("border: 2px solid white");
    //resultDisplay->show();

    vp = new mpvWidget(this);
    vp->resize(size());
    vp->setLoop(false);
    vp->setProperty("pause", false);
    vp->setProperty("keep-open",true);
    vp->hide();


    int y0 = 747;
    earthDisplay = new QLabel(vp);
    earthDisplay->resize(300,50);
  //  earthDisplay->setStyleSheet("border: 2px solid white");
    earthDisplay->move(550,y0);
    earthDisplay->show();

    moonDisplay = new QLabel(vp);
    moonDisplay->resize(300,50);
  //  moonDisplay->setStyleSheet("border: 2px solid white");
    moonDisplay->move(550,y0+155);
    moonDisplay->show();

    marsDisplay = new QLabel(vp);
    marsDisplay->resize(300,50);
   // marsDisplay->setStyleSheet("border: 2px solid white");
    marsDisplay->move(550,y0+2*155);
    marsDisplay->show();

    jupiterDisplay = new QLabel(vp);
    jupiterDisplay->resize(300,50);
   // jupiterDisplay->setStyleSheet("border: 2px solid white");
    jupiterDisplay->move(550,y0+3*155);
    jupiterDisplay->show();

    font = QFont(PATH+"font.ttf",110);
    font2 = QFont(PATH+"font.ttf",30);

    setupStates();

}



void mainScreen::setupStates()
{
    idle = new QState();
    rules = new QState();
    cd = new QState();
    jump = new QState();
    result = new QState();
    videoresult = new QState();

    stateTimer = new QTimer(this);
    stateTimer->setSingleShot(true);
    stateTimer->setInterval(5000);

    connect(idle,SIGNAL(entered()),this,SLOT(goIdle()));

    idle->addTransition(this,SIGNAL(gotJump(int)),rules);
    connect(rules,SIGNAL(entered()),this,SLOT(goRules()));


    rules->addTransition(this,SIGNAL(countDownOver()),cd);
    connect(cd,SIGNAL(entered()),this,SLOT(goCountDown()));

    cd->addTransition(this,SIGNAL(countDownOver()),jump);
    connect(jump,SIGNAL(entered()),this,SLOT(goJump()));

    jump->addTransition(this,SIGNAL(gotValidJump()),result);
    connect(result,SIGNAL(entered()),this,SLOT(goResult()));


    result->addTransition(stateTimer,SIGNAL(timeout()),videoresult);
    connect(this,SIGNAL(displayOver()),stateTimer,SLOT(start()));


    connect(videoresult,SIGNAL(entered()),this,SLOT(showVideo()));
    videoresult->addTransition(vp,SIGNAL(videoOver()),idle);


    machine.addState(idle);
    machine.addState(rules);
    machine.addState(cd);
    machine.addState(jump);
    machine.addState(result);
    machine.addState(videoresult);

    machine.setInitialState(idle);
    machine.start();
}



void mainScreen::loadCoefs()
{

    earth = 57;
    moon = 0.165;
    jupiter = 2.52;
    mars = 0.378;



    QFile file(PATH+"config.cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"no config file";

    }
    else
    {


        QTextStream in(&file);

        QString  line;
        QStringList params;
        QString paramName,paramValue;
        while(!in.atEnd()) {
            line = in.readLine();

            params = line.split("=");
            if(params.size()>=2)
            {
                paramName = params[0];
                paramValue = params[1];

                if (paramName=="EARTH")
                {
                    earth = paramValue.toInt();
                    qDebug()<<"earth "<<earth;
                }


            }



        }
        file.close();

    }

}




void mainScreen::goIdle()
{
    resultDisplay->hide();
    vp->hide();
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


    painter.setFont(font);

    QFontMetrics fm(font);

    double W = fm.width("100");
    double H = fm.height();


    painter.drawText(QRectF((resultDisplay->width()-W)/2,(resultDisplay->height()-H)/2-20,width(),height()),text);

    QFont font2 = QFont(PATH+"font.ttf",50);
    painter.setFont(font2);
    W = QFontMetrics(font2).width("cm");
    painter.drawText(QRectF((resultDisplay->width()-W)/2,(resultDisplay->height()-H)/2+110,width(),height()),"cm");



    resultDisplay->setPixmap(pix);


}






void mainScreen::getJump(int time)
{
    qDebug()<<"jump "<<time;

    if((machine.configuration().contains(jump))&&(time>30))
    {
        double h = (double)time*earth/1000;

        jumpHeight = h;
        moonScore = (double)h/moon;
        earthScore = h;
        marsScore = (double)h/mars;
        jupiterScore = (double)h/jupiter;

        emit gotValidJump();
    }
}

void mainScreen::showVideo()
{
    jupiterDisplay->hide();
    moonDisplay->hide();
    earthDisplay->hide();
    marsDisplay->hide();
    vp->show();
    vp->raise();
    vp->loadFile(PATH+"video.mp4");
    vp->setProperty("pause", false);


    QTimer::singleShot(2200,this,SLOT(showEarth()));
    QTimer::singleShot(3400,this,SLOT(showMoon()));
    QTimer::singleShot(4450,this,SLOT(showMars()));
    QTimer::singleShot(5450,this,SLOT(showJupiter()));

}



void mainScreen::showEarth()
{

    QString text = QString("%1").arg(earthScore, 3, 10, QChar('0')) + " cm";
    QPixmap pix(earthDisplay->size());
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 0));
    painter.setFont(font2);
    QFontMetrics fm(font2);
    double W = fm.width("100 cm");
    double H = fm.height();
    painter.drawText(QRectF((earthDisplay->width()-W)/2,(earthDisplay->height()-H)/2,width(),height()),text);
    earthDisplay->setPixmap(pix);
    earthDisplay->show();
}

void mainScreen::showMoon()
{
    QString text = QString("%1").arg(moonScore, 3, 10, QChar('0')) + " cm";
    QPixmap pix(moonDisplay->size());
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 0));
    painter.setFont(font2);
    QFontMetrics fm(font2);
    double W = fm.width("100 cm");
    double H = fm.height();
    painter.drawText(QRectF((moonDisplay->width()-W)/2,(moonDisplay->height()-H)/2,width(),height()),text);
    moonDisplay->setPixmap(pix);
    moonDisplay->show();
}

void mainScreen::showJupiter()
{
    QString text = QString("%1").arg(jupiterScore, 3, 10, QChar('0')) + " cm";
    QPixmap pix(jupiterDisplay->size());
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 0));
    painter.setFont(font2);
    QFontMetrics fm(font2);
    double W = fm.width("100 cm");
    double H = fm.height();
    painter.drawText(QRectF((jupiterDisplay->width()-W)/2,(jupiterDisplay->height()-H)/2,width(),height()),text);
    jupiterDisplay->setPixmap(pix);
    jupiterDisplay->show();
}

void mainScreen::showMars()
{
    QString text = QString("%1").arg(marsScore, 3, 10, QChar('0')) + " cm";
    QPixmap pix(marsDisplay->size());
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 0));
    painter.setFont(font2);
    QFontMetrics fm(font2);
    double W = fm.width("100 cm");
    double H = fm.height();
    painter.drawText(QRectF((marsDisplay->width()-W)/2,(marsDisplay->height()-H)/2,width(),height()),text);
    marsDisplay->setPixmap(pix);
    marsDisplay->show();
}



void mainScreen::mousePressEvent(QMouseEvent *ev)
{
    qDebug()<<ev->pos();

}
