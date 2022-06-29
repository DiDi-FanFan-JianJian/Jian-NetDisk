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
    // ��ֹʹ������
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
    if (password.size() < 12 || password.size() > 24)
        return true;

    int flag = 0;
    // ����Сд��ĸ
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= 'a') && (password[i] <= 'z'))
        {
            ++flag;
            break;
        }
    // ������д��ĸ
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= 'A') && (password[i] <= 'Z'))
        {
            ++flag;
            break;
        }
    // ��������
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= '0') && (password[i] <= '9'))
        {
            ++flag;
            break;
        }
    // ���������ַ�
    for (int i = 0; i < password.size(); ++i)
        if ((password[i] >= '!') && (password[i] <= '/'))
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
        showMsg(QStringLiteral("����д������"));
        return;
    }
    if (sign_up_password1 != sign_up_password2) {
        showMsg(QStringLiteral("������������벻һ��"));
        return;
    }
    if (isWeak(sign_up_password1)) {
        showMsg(QStringLiteral("����ǿ��̫�������볤��Ϊ12-24λ�����ٰ�������/Сд��ĸ/��д��ĸ/�����ַ��е����֣�"));
        return;
    }

    // �ж�socket�Ƿ����ӣ�δ��������������
    sock->sendUserInfo(MSG_TYPE_REGISTER, sign_up_account.toStdString(), sign_up_password1.toStdString());
    sock->recvMsg();
    auto res = LoginResponse(sock->recv_buf);
    if (res.status == LoginResponse::success) {
        showMsg(QStringLiteral("ע��ɹ�"));
        this->accept();
    }
    else if (res.status == LoginResponse::user_exist) {
        showMsg(QStringLiteral("���û����Ѵ���!"));
    }
    else {
        showMsg(QStringLiteral("ע��ʧ��"));
    }
}
