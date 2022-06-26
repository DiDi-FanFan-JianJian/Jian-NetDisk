#ifndef SIGNINWIDGET_H
#define SIGNINWIDGET_H

#include <QWidget>
#include <QMessageBox>

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
    Ui::SignInWidget *ui;
    // 连接的服务端
    QString ip, port;
    // 登录的用户名、密码
    QString sign_in_account, sign_in_password;

    void ShowMsg(QString msg);
    void Reconnect();
};

#endif // SIGNINWIDGET_H
