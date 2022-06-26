#include "netdisk.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetDisk w;
//    SignInWidget w;
    w.show();
    return a.exec();
}
