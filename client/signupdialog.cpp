#include "signupdialog.h"
#include "ui_signupdialog.h"

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
    ui->sign_up_account->setAttribute(Qt::WA_InputMethodEnabled, false);
}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}

void SignUpDialog::on_sign_up_account_textChanged(const QString &arg1)
{
    this->sign_up_account = arg1;
}

void SignUpDialog::on_sign_up_password1_textChanged(const QString &arg1)
{
    this->sign_up_password1 = arg1;
}

void SignUpDialog::on_sign_up_password2_textChanged(const QString &arg1)
{
    this->sign_up_password2 = arg1;
}

void SignUpDialog::ShowMsg(const QString msg)
{
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

bool SignUpDialog::IsWeak(const QString password)
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
    if (sign_up_account.trimmed().isEmpty() || sign_up_password1.trimmed().isEmpty() || sign_up_password2.trimmed().isEmpty())
    {
        ShowMsg("表单填写不完整");
        return;
    }
    if (sign_up_password1 != sign_up_password2)
    {
        ShowMsg("两次输入的密码不一致");
        return;
    }
    if (IsWeak(sign_up_password1))
    {
        ShowMsg("密码强度太弱（密码长度为8-16位，必须同时包含数字/小写字母/大写字母）");
        return;
    }
    this->accept();
}
