#include "netdisk.h"
#include "ui_netdisk.h"
#include "mysocket.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

extern SJ::MySocket& my_socket;

NetDisk::NetDisk(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetDisk)
{
    ui->setupUi(this);
}

NetDisk::~NetDisk()
{
    delete ui;
}


void NetDisk::on_refresh_btn_clicked() {
    showMsg("刷新列表");
}


void NetDisk::on_return_btn_clicked() {
    showMsg("返回上一级");

}

void NetDisk::on_upload_file_clicked() {
    // getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, Options options)
    //                      父类指针                标题              默认打开文件夹         文件过滤规则          默认的文件过滤规则         属性（显示文件夹等）
    //  filter："images(*.png *jpeg *bmp);;text files(*.txt *.doc *.docx);;video files(*.avi *.mp4 *.wmv);;All files(*.*)"
    QString file_name = QFileDialog::getOpenFileName(this, "选择文件", "D:/", "images(*.png *jpeg *bmp)");
    if (file_name.isEmpty()) {

    }
    else {
        showMsg(file_name);
    }
}


void NetDisk::on_upload_dir_clicked() {
    showMsg("上传文件夹");
}

void NetDisk::showMsg(QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

void NetDisk::on_new_dir_clicked()
{
    showMsg("新建文件夹");
}


void NetDisk::on_rename_btn_clicked()
{
    showMsg("重命名");
}


void NetDisk::on_copy_btn_clicked()
{
    showMsg("复制");
}

void NetDisk::on_delete_btn_clicked()
{
    showMsg("删除");
}

void NetDisk::on_move_btn_clicked()
{
    showMsg("移动");
}

