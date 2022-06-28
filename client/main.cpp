#include "signinwidget.h"
#include "netdisk.h"
#include "mysocket.h"

#include <QApplication>

SJ::MySocket sock("1.15.144.212", 8000);

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    SignInWidget w;
//    NetDisk w;
    w.show();
    return a.exec();
}
