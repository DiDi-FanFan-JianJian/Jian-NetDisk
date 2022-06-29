#include "signinwidget.h"
#include "netdisk.h"
#include "mysocket.h"
#include "global_msg.h"

#include <QApplication>
#include <QTextCodec>

Global_Msg g_msg;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // gbk
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
    SignInWidget w;
    w.show();
    return a.exec();
}
