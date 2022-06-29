#ifndef SIGNINWIDGET_H
#define SIGNINWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <iostream>

#include "mysocket.h"
#include "global_msg.h"

namespace Ui {
class SignInWidget;
}

class SignInWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SignInWidget(QWidget *parent = nullptr);
    ~SignInWidget();

private slots:

    void on_sign_in_account_textChanged(const QString &arg1);

    void on_sign_in_password_textChanged(const QString &arg1);

    void on_go_to_sign_up_clicked();

    void on_sign_in_btn_clicked();

private:
    SJ::MySocket* sock;
    Ui::SignInWidget *ui;

    // 登录的用户名、密码
    QString sign_in_account, sign_in_password;

    void showMsg(QString msg);
    void reConnect();
};

#endif // SIGNINWIDGET_H
