#include "signinwidget.h"
#include "netdisk.h"
#include "mysocket.h"
#include "global_msg.h"

#include <QApplication>

Global_Msg g_msg;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    SignInWidget w;
    w.show();
    return a.exec();
}
