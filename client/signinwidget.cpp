#include "signinwidget.h"
#include "ui_signinwidget.h"
#include "signupdialog.h"
#include "netdisk.h"
#include "mysocket.h"

extern SJ::MySocket& my_socket;

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
    // 如果用户名和密码都不为空，则发送登录请求
    if (sign_in_account.isEmpty() || sign_in_password.isEmpty()) {
        showMsg("请输入用户名和密码");
        return;
    }

    // 发送登录请求，获取登录结果
    showMsg(sign_in_account + " " + sign_in_password);
    int ret = 1;

    if (ret) {
        // 登录成功，到主页面
        NetDisk *client = new NetDisk();
        this->close();
        client->show();
    }
    else {
        // 登录失败，提示错误信息
        showMsg("登录失败原因");
        reConnect();
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
        showMsg(this->sign_in_account + " " + this->sign_in_password);
        ui->sign_in_account->setText(sign_dlg.sign_up_account);
        ui->sign_in_password->setText(sign_dlg.sign_up_password1);
        this->show();
        reConnect();
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
