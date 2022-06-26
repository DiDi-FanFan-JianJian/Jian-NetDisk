#include "netdisk.h"
#include "ui_netdisk.h"
#include <QDebug>
#include <QMessageBox>

NetDisk::NetDisk(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetDisk)
{
    ui->setupUi(this);
    test = new QAction("test", ui->file_list);   //因为是在QTreeWidget内实现右键，所以以其作为父类

    connect(ui->file_list, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(itemPressedSlot(QTreeWidgetItem*,int)));
    connect(test, SIGNAL(triggered(bool)), this , SLOT(testSlot()));
}

NetDisk::~NetDisk()
{
    delete ui;
}


void NetDisk::on_refresh_btn_clicked()
{
    showMsg("刷新列表");
}


void NetDisk::on_return_btn_clicked()
{
    showMsg("返回上一级");

}

void NetDisk::on_upload_file_clicked()
{
    showMsg("上传文件");
}


void NetDisk::on_upload_dir_clicked()
{
    showMsg("上传文件夹");
}

void NetDisk::showMsg(QString msg)
{
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

void NetDisk::testSlot()
{
    qDebug() << "Hello, Action";
}
