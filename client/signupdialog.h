#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class SignUpDialog;
}

class SignUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignUpDialog(QWidget *parent = nullptr);
    ~SignUpDialog();
    // 注册用户名、密码
    QString sign_up_account, sign_up_password1, sign_up_password2;

private slots:
    void on_sign_up_account_textChanged(const QString &arg1);

    void on_sign_up_password1_textChanged(const QString &arg1);

    void on_sign_up_password2_textChanged(const QString &arg1);

    void on_sign_up_btn_clicked();

private:
    Ui::SignUpDialog *ui;
    bool isWeak(const QString pw);
    void showMsg(const QString msg);
};

#endif // SIGNUPDIALOG_H
