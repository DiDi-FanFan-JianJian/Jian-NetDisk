#include <iostream>

#include "signupdialog.h"
#include "ui_signupdialog.h"

#include "message.h"
#include "global_msg.h"

extern Global_Msg g_msg;

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    this->sock = new SJ::MySocket("1.15.144.212", 8000);
    ui->setupUi(this);
    // 禁止使用中文
    ui->sign_up_account->setAttribute(Qt::WA_InputMethodEnabled, false);
}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}

void SignUpDialog::on_sign_up_account_textChanged(const QString &arg1)
{
    this->sign_up_account = arg1.trimmed();
}

void SignUpDialog::on_sign_up_password1_textChanged(const QString &arg1)
{
    this->sign_up_password1 = arg1.trimmed();
}

void SignUpDialog::on_sign_up_password2_textChanged(const QString &arg1)
{
    this->sign_up_password2 = arg1.trimmed();
}

void SignUpDialog::showMsg(const QString msg)
{
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

bool SignUpDialog::isWeak(const QString password)
{
    if (password.size() < 8 || password.size() > 16)
        return true;

    int flag = 0;
    // 包含小写字母
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= 'a') && (password[i] <= 'z'))
        {
            ++flag;
            break;
        }
    // 包含大写字母
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= 'A') && (password[i] <= 'Z'))
        {
            ++flag;
            break;
        }
    // 包含数字
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= '0') && (password[i] <= '9'))
        {
            ++flag;
            break;
        }
    if (flag < 3)
        return true;
    return false;
}

void SignUpDialog::on_sign_up_btn_clicked()
{
    if (sign_up_account.isEmpty() || sign_up_password1.isEmpty() || sign_up_password2.isEmpty()) {
        showMsg("表单填写不完整");
        return;
    }
    if (sign_up_password1 != sign_up_password2) {
        showMsg("两次输入的密码不一致");
        return;
    }
    if (isWeak(sign_up_password1)) {
        showMsg("密码强度太弱（密码长度为8-16位，必须同时包含数字/小写字母/大写字母）");
        return;
    }

    // 判断socket是否连接，未连接则重新连接
    sock->sendUserInfo(MSG_TYPE_REGISTER, sign_up_account.toStdString(), sign_up_password1.toStdString());
    sock->recvMsg();
    auto res = LoginResponse(sock->recv_buf);
    if (res.status == LoginResponse::success) {
        showMsg("注册成功");
        this->accept();
    }
    else if (res.status == LoginResponse::user_exist) {
        showMsg("该用户名已存在!");
    }
    else {
        showMsg("注册失败");
    }
}
