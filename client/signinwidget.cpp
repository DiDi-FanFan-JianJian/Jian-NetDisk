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
    // ����û��������붼��Ϊ�գ����͵�¼����
    if (sign_in_account.isEmpty() || sign_in_password.isEmpty()) {
        showMsg("�������û���������");
        return;
    }
    // ���͵�¼���󣬻�ȡ��¼���
    sock->sendUserInfo(MSG_TYPE_LOGIN, this->sign_in_account.toStdString(), this->sign_in_password.toStdString());
    sock->recvMsg();
    auto res = LoginResponse(sock->recv_buf);
    if (res.status == LoginResponse::success) {
        // ��¼�ɹ�������ҳ��
        showMsg("��½�ɹ�!");
        // ��¼ȫ����Ϣ
        g_msg.username = this->sign_in_account.toStdString();
        g_msg.path.push_back("root");
        g_msg.cur_dir = res.dir;

        NetDisk *client = new NetDisk();
        this->close();
        client->show();
    }
    else if (res.status == LoginResponse::passwd_error) {
        showMsg("�������!");
    }
    else {
        showMsg("�û�������!");
    }
}

void SignInWidget::on_go_to_sign_up_clicked()
{
    /* 
     * ����һ���µĴ��ڣ���ʾע��Ի���
     *
     * �Ի��򷵻������������ע��ɹ�����
     *                     �رմ��ڷ���
     */
    SignUpDialog sign_dlg;
    this->hide();
    if (sign_dlg.exec() == QDialog::Accepted) {
        // ע��ɹ����أ��Զ���д�û���������
        ui->sign_in_account->setText(sign_dlg.sign_up_account);
        ui->sign_in_password->setText(sign_dlg.sign_up_password1);
        this->show();
    }
    else {
        // �رմ��ڷ��أ������κδ���
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
    // �����¼ʧ�ܣ�����������
    // my_socket.reConnect();
}
