#include "signinwidget.h"
#include "ui_signinwidget.h"
#include "signupdialog.h"
#include "netdisk.h"
#include "mysocket.h"
#include "message.h"
#include "global_msg.h"

extern Global_Msg g_msg;

SignInWidget::SignInWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignInWidget)
{
    this->sock = new SJ::MySocket("1.15.144.212", 8000);
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
    // 如果用户名和密码都不为空，则发送登录请求
    if (sign_in_account.isEmpty() || sign_in_password.isEmpty()) {
        showMsg("请输入用户名和密码");
        return;
    }
    // 发送登录请求，获取登录结果
    sock->sendUserInfo(MSG_TYPE_LOGIN, this->sign_in_account.toStdString(), this->sign_in_password.toStdString());
    sock->recvMsg();
    auto res = LoginResponse(sock->recv_buf);
    if (res.status == LoginResponse::success) {
        // 登录成功，到主页面
        showMsg("登陆成功!");
        // 记录全局信息
        g_msg.username = this->sign_in_account.toStdString();
        g_msg.path.push_back("root");
        g_msg.cur_dir = res.dir;

        NetDisk *client = new NetDisk();
        this->close();
        client->show();
    }
    else if (res.status == LoginResponse::passwd_error) {
        showMsg("密码错误!");
    }
    else {
        showMsg("用户不存在!");
    }
}

void SignInWidget::on_go_to_sign_up_clicked()
{
    /* 
     * 创建一个新的窗口，显示注册对话框
     *
     * 对话框返回有两种情况：注册成功返回
     *                     关闭窗口返回
     */
    SignUpDialog sign_dlg;
    this->hide();
    if (sign_dlg.exec() == QDialog::Accepted) {
        // 注册成功返回，自动填写用户名和密码
        ui->sign_in_account->setText(sign_dlg.sign_up_account);
        ui->sign_in_password->setText(sign_dlg.sign_up_password1);
        this->show();
    }
    else {
        // 关闭窗口返回，不做任何处理
        this->show();
    }
}

void SignInWidget::showMsg(QString msg)
{
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

void SignInWidget::reConnect()
{
    // 如果登录失败，则重新连接
    // my_socket.reConnect();
}
