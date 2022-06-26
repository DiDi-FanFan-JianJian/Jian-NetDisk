#include "signinwidget.h"
#include "netdisk.h"
#include "mysocket.h"

#include <QApplication>

SJ::MySocket my_socket;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SignInWidget w;
    w.show();
    return a.exec();
}
