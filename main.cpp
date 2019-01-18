#include <QApplication>
#include "qwindow.h"
#include "qscreen.h"
#include "qwidget.h"
#include "mainscreen.h"
#include "qdebug.h"



#include "serialwatcher.h"

#define PATH_DEFAULT (QString)"/home/fred/Dropbox/Taf/Cassiopee/jump/files/"
#define TIMEOUT 15000

#define defaultSpeed (10)

int main(int argc, char *argv[])

{
    QApplication a(argc, argv);




    QString PATH;
    QStringList params = a.arguments();
    if(params.size()>1)
        PATH = params[1];
    else
        PATH=PATH_DEFAULT;

    bool HIDE_CURSOR=false;



    QFile file(PATH+"config.cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"no config file";

    }
    else
    {

        QTextStream in(&file);

        QString  line;
        QString paramName,paramValue;
        QStringList params;

        while(!in.atEnd()) {
            line = in.readLine();
            line = (line.split("#"))[0];
            params = line.split("=");
            if(params.size()>=2)
            {
                paramName = params[0];
                paramValue = params[1];

                if (paramName.mid(0,6)=="CURSOR")
                    HIDE_CURSOR = (paramValue=="NO");

                else
                    qDebug()<<paramName<<" - "<<paramValue;
            }
        }
        file.close();

    }




    if (HIDE_CURSOR)
    {
        QCursor cursor(Qt::BlankCursor);
        a.setOverrideCursor(cursor);
        a.changeOverrideCursor(cursor);
    }


    serialWatcher * serialwatch = new serialWatcher(NULL);



    qDebug()<<"Screens count: "<<a.screens().size();

    mainScreen * ms = new mainScreen(NULL,PATH);



    ms->setGeometry(a.screens()[2]->geometry().x(),a.screens()[2]->geometry().y(),1080,1920);
    ms->show();

    a.connect(serialwatch,SIGNAL(getJumpTime(int)),ms,SLOT(getJump(int)));
    a.connect(serialwatch,SIGNAL(getJumpTime(int)),ms,SIGNAL(gotJump(int)));




    return a.exec();

}
