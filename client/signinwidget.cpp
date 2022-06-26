#include "signinwidget.h"
#include "ui_signinwidget.h"
#include "signupdialog.h"
#include "netdisk.h"

SignInWidget::SignInWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignInWidget)
{
    ui->setupUi(this);
    ui->sign_in_account->setAttribute(Qt::WA_InputMethodEnabled, false);
}

SignInWidget::~SignInWidget()
{
    delete ui;
}

void SignInWidget::on_sign_in_account_textChanged(const QString &arg1)
{
    this->sign_in_account = arg1;
}

void SignInWidget::on_sign_in_password_textChanged(const QString &arg1)
{
    this->sign_in_password = arg1;
}

void SignInWidget::on_sign_in_btn_clicked()
{
    SJ::SignInReq req;
    req.password = sign_in_password.toStdString();
    req.username = sign_in_account.toStdString();
    // req.session = session.toStdString();

    // log("LoginWindow: Logining in as " + sign_in_account);

//    SJ::SignInRes res;
//    res.code = 0;
    // controller_p->login(req, res);
//    if (res.code == 0)
    if (1)
    {
//        QFile file("./tmyuser.txt");
//        file.open(QIODevice::WriteOnly | QIODevice::Text);
//        QTextStream ts(&file);
//        session = QString(res.session.c_str());
//        ts << ip << "\n" << ports << "\n" << session << "\n" << syncPath;
//        file.close();

        NetDisk *client = new NetDisk();
//        client.initialize(syncPath, controller_p);
//        client.exec();
        this->close();
        client->show();
    }
    else
    {
//        log("LoginWindowLogin in failed, msg: " + QString(res.message.c_str()));
        ShowMsg(QString(res.message.c_str()));
        Reconnect();
    }

}

void SignInWidget::on_go_to_sign_up_clicked()
{
    // 启动注册的dialog
    SignUpDialog sign_dlg;
    this->hide();
    if (sign_dlg.exec() == QDialog::Accepted)
    {
        // SJ::SignUpReq req;
        // req.username = sign_dlg.sign_up_account.toStdString();
        // req.password = sign_dlg.sign_up_password1.toStdString();
        // SJ::SignUpRes res;
        // controller_p->signup(req, res);
        this->show();
        ShowMsg(sign_dlg.sign_up_account + sign_dlg.sign_up_password1);
        Reconnect();
    }
    else
    {
        this->show();
    }
}

void SignInWidget::ShowMsg(QString msg)
{
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

void SignInWidget::Reconnect()
{

}
