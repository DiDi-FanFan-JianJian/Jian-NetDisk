#include "netdisk.h"
#include "ui_netdisk.h"
#include "mysocket.h"
#include "transferlistdialog.h"

#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "global_msg.h"

extern Global_Msg g_msg;

// 添加测试文件/文件夹
void NetDisk::reloadFile()
{
    dir_list.clear();
    file_list.clear();
    auto list1 = this->sock->get_cur_dirs();
    for (auto it: list1) {
        dir_list.append(QString::fromStdString(it));
    }
    auto list2 = this->sock->get_cur_files();
    for (auto it: list2) {
        file_list.append(QString::fromStdString(it));
    }
    // 渲染文件列表
    renderFileList(file_list, dir_list);
}


NetDisk::NetDisk(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetDisk)
{
    this->sock = new SJ::MySocket("1.15.144.212", 8000);
    g_msg.read_file_list();
    ui->setupUi(this);
    path = "root/";
    ui->now_path->setText(path);
    reloadFile();
}

NetDisk::~NetDisk()
{
    delete ui;
}

void NetDisk::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("退出程序"),QString(tr("确认退出程序")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No) {
        event->ignore(); // 忽略退出信号，程序继续进行
    }
    else if(button==QMessageBox::Yes) {
        event->accept(); // 接受退出信号，程序退出
    }
    g_msg.write_file_list();
}

void NetDisk::on_refresh_btn_clicked() {
    // 渲染文件列表
    ui->now_path->setText(path);
    reloadFile();
    renderFileList(file_list, dir_list);
}

void NetDisk::on_return_btn_clicked() {
    g_msg.go_back();
    path = QString::fromStdString(g_msg.get_path());
    // 渲染文件列表
    ui->now_path->setText(path);
    reloadFile();
    renderFileList(file_list, dir_list);
}

// 缺少文件路径分析
void NetDisk::on_upload_file_clicked() {
    // getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, Options options)
    //                      父类指针                标题              默认打开文件夹         文件过滤规则          默认的文件过滤规则         属性（显示文件夹等）
    //  filter："images(*.png *jpeg *bmp);;text files(*.txt *.doc *.docx);;video files(*.avi *.mp4 *.wmv);;All files(*.*)"

    // 选择文件（限制只能选择文件）
    QString file_path = QFileDialog::getOpenFileName(this, "选择文件", ".", "All files(*.*)");
    if (file_path.isEmpty()) {
        showMsg("你自己关了，无事发生");
        return;
    }
    else {
        showMsg("选择文件：" + file_path);

        file_list.append(file_path);
        renderFileList(file_list, dir_list);
    }
}

// 缺少文件路径分析
void NetDisk::on_upload_dir_clicked() {
    // 选择文件夹（限制只能选择文件夹）
    QString dir_name = QFileDialog::getExistingDirectory(this, "选择文件夹", "D:/");
    if (dir_name.isEmpty()) {
        showMsg("你自己关了，无事发生");
    }
    else {
        showMsg(dir_name);

        dir_list.append(dir_name);
        renderFileList(file_list, dir_list);
    }
}

void NetDisk::on_new_dir_clicked()
{
    // 弹框询问文件夹名称
    bool ok;
    QString dir_name = QInputDialog::getText(this, QStringLiteral("新建文件夹"), QStringLiteral("请输入文件夹名称"), QLineEdit::Normal, QString(""), &ok);
    if (ok) {
        if (dir_name.isEmpty()) {
            showMsg(QStringLiteral("输入不能为空"));
        }
        else {
            showMsg(dir_name);
            this->sock->create_dir(dir_name.toStdString());
            reloadFile();
            renderFileList(file_list, dir_list);
        }
    }
}


void NetDisk::on_paste_btn_clicked()
{
    // 判断有没有复制
    if (g_msg.copyfile_status == PASTE_NOFILE) {
        showMsg("请先复制文件");
        return;
    }

    QString dir_id = QString(g_msg.copyfile_dir_id);
    if (g_msg.copyfile_status == PASTE_COPYFILE) {
        // 复制文件
        this->sock->copy_file(g_msg.get_cur_id(), g_msg.copyfile_id, g_msg.copyfile_name);
    }
    else if (g_msg.copyfile_status == PASTE_COPYDIR) {
        // 复制文件夹
        this->sock->copy_dir(g_msg.copyfile_id);
    }
    else if (g_msg.copyfile_status == PASTE_CUTFILE) {
        // 剪切文件
        this->sock->move_file(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    else {
        // 剪切文件夹
        this->sock->move_dir(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    reloadFile();
    renderFileList(file_list, dir_list);
}

// 渲染文件列表
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // 使用QTableWidget控件显示文件列表
    ui->file_list->clear();
    ui->file_list->setColumnCount(7);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    ui->file_list->setHorizontalHeaderLabels(QStringList() << "文件名" << "大小" << "下载" << "删除" << "重命名" << "复制" << "剪切");
    for (int i = 0; i < file_list.size(); i++) {
        // 文件名，给file_name设置icon，并且设置不可编辑
        QTableWidgetItem *file_name = new QTableWidgetItem(file_list.at(i));
        QIcon icon(":img/file_icon.png");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 0, file_name);
        // 文件大小
        QTableWidgetItem *file_size = new QTableWidgetItem("1.0KB");
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 1, file_size);

        QPixmap *pixmap;
        // 下载图案
        pixmap = new QPixmap(":img/download.png");
        QLabel *download_label = new QLabel();
        QPixmap download_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        download_label->setPixmap(download_img);
        download_label->setAlignment(Qt::AlignCenter);
        ui->file_list->setCellWidget(i, 2, download_label);
        delete pixmap;
        // 删除图案
        pixmap = new QPixmap(":img/delete.png");
        QLabel *delete_label = new QLabel();
        QPixmap delete_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        delete_label->setPixmap(delete_img);
        delete_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 3, delete_label);
        delete pixmap;
        // 重命名图案
        pixmap = new QPixmap(":img/rename.png");
        QLabel *rename_label = new QLabel();
        QPixmap rename_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        rename_label->setPixmap(rename_img);
        rename_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 4, rename_label);
        delete pixmap;
        // 复制图案
        pixmap = new QPixmap(":img/copy.png");
        QLabel *copy_label = new QLabel();
        QPixmap copy_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        copy_label->setPixmap(copy_img);
        copy_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 5, copy_label);
        delete pixmap;
        // 剪切图案
        pixmap = new QPixmap(":img/cut.png");
        QLabel *cut_label = new QLabel();
        QPixmap cut_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cut_label->setPixmap(cut_img);
        cut_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 6, cut_label);
        delete pixmap;
    }
    for (int i = 0; i < dir_list.size(); i++) {
        // 文件名，给file_name设置icon
        QTableWidgetItem *file_name = new QTableWidgetItem(dir_list.at(i));
        QIcon icon(":img/dir_icon.jpg");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 0, file_name);
        // 文件大小
        QTableWidgetItem *file_size = new QTableWidgetItem("文件夹");
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 1, file_size);
        QPixmap *pixmap;
        // 下载图案
        pixmap = new QPixmap(":img/download.png");
        QLabel *download_label = new QLabel();
        QPixmap download_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        download_label->setPixmap(download_img);
        download_label->setAlignment(Qt::AlignCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 2, download_label);
        delete pixmap;
        // 删除图案
        pixmap = new QPixmap(":img/delete.png");
        QLabel *delete_label = new QLabel();
        QPixmap delete_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        delete_label->setPixmap(delete_img);
        delete_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 3, delete_label);
        delete pixmap;
        // 重命名图案
        pixmap = new QPixmap(":img/rename.png");
        QLabel *rename_label = new QLabel();
        QPixmap rename_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        rename_label->setPixmap(rename_img);
        rename_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 4, rename_label);
        delete pixmap;
        // 复制图案
        pixmap = new QPixmap(":img/copy.png");
        QLabel *copy_label = new QLabel();
        QPixmap copy_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        copy_label->setPixmap(copy_img);
        copy_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 5, copy_label);
        delete pixmap;
        // 剪切图案
        pixmap = new QPixmap(":img/cut.png");
        QLabel *cut_label = new QLabel();
        QPixmap cut_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cut_label->setPixmap(cut_img);
        cut_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 6, cut_label);
        delete pixmap;
    }
}

// 显示消息
void NetDisk::showMsg(QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

// table中的按钮
void NetDisk::on_file_list_cellClicked(int row, int column)
{
    if (column < 2 ) {
        // 点击第一列、第二列无效果
        return;
    }

    // 计算文件/文件夹的id
    int item_id, is_dir;
    if (row < file_list.length()) {
        item_id = row;
        is_dir = 0;
    }
    else {
        item_id = row - file_list.length();
        is_dir = 1;
    }

    // 执行对应操作
    if (column == 2) {
        // 下载文件/文件夹
        QString download_path = QFileDialog::getExistingDirectory(this, "选择文件夹", "C:/");
        if (download_path.isEmpty()) {
            // 取消或者关闭对话框
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            showMsg("下载文件夹" + dir_name + "到指定位置" + download_path);
        }
        else {
            QString file_name = file_list.at(item_id);
            showMsg("下载文件" + file_name + "到指定位置" + download_path);
        }
    }
    else if (column == 3) {
        // 删除文件/文件夹
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            // 询问是否删除
            QMessageBox msgbox;
            msgbox.setText("Delete " + dir_name + "?");
            msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgbox.setDefaultButton(QMessageBox::No);
            int ret = msgbox.exec();
            if (ret == QMessageBox::Yes) {
                // 删除文件夹
                showMsg("删除文件夹" + dir_name);
                int id = this->sock->get_dir_id(dir_name.toStdString());
                this->sock->delete_dir(id);
                cout << dir_name.toStdString() << " " << id << endl;
            }
        }
        else {
            QString file_name = file_list.at(item_id);
            // 询问是否删除
            QMessageBox msgbox;
            msgbox.setText("Delete " + file_name + "?");
            msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgbox.setDefaultButton(QMessageBox::No);
            int ret = msgbox.exec();
            if (ret == QMessageBox::Yes) {
                // 删除文件
                showMsg("删除文件" + file_name);

                ui->file_list->removeRow(row);
                file_list.removeAt(item_id);
            }
        }
    }
    else if (column == 4) {
        // 重命名
        bool ok;
        QString new_name = QInputDialog::getText(this, "重命名", "请输入新的文件名", QLineEdit::Normal, QString(""), &ok);
        if (!ok) {
            // 点击取消或者直接关闭
            return;
        }
        if (new_name.isEmpty()) {
            showMsg(QStringLiteral("输入不能为空"));
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            showMsg("重命名文件夹" + dir_name + "为" + new_name);
            int id = this->sock->get_dir_id(dir_name.toStdString());
            this->sock->rename_dir(id, new_name.toStdString());
        }
        else {
            QString file_name = file_list.at(item_id);
            showMsg("重命名文件" + file_name + "为" + new_name);
            int id = this->sock->get_file_id(file_name.toStdString());
            this->sock->rename_file(id, new_name.toStdString());
        }
    }
    else if (column == 5) {
        // 复制
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            // showMsg("copy dir: " + dir_name);
            g_msg.copyfile_name = dir_name.toStdString();
            g_msg.copyfile_id = this->sock->get_dir_id(g_msg.copyfile_name);
            g_msg.copyfile_dir_id = g_msg.get_cur_id();
            g_msg.copyfile_status = PASTE_COPYDIR;
            cout << g_msg.copyfile_dir_id;
        }
        else {
            QString file_name = file_list.at(item_id);
            // showMsg("copy file: " + file_name);
            g_msg.copyfile_name = file_name.toStdString();
            g_msg.copyfile_id = this->sock->get_file_id(g_msg.copyfile_name);
            g_msg.copyfile_dir_id = g_msg.get_cur_id();
            g_msg.copyfile_status = PASTE_COPYFILE;
        }
    }
    else if (column == 6) {
        // 剪切
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            // showMsg("cut dir: " + dir_name);
            g_msg.copyfile_name = dir_name.toStdString();
            g_msg.copyfile_id = this->sock->get_dir_id(g_msg.copyfile_name);
            g_msg.copyfile_dir_id = g_msg.get_cur_id();
            g_msg.copyfile_status = PASTE_CUTDIR;
        }
        else {
            QString file_name = file_list.at(item_id);
            // showMsg("cut file: " + file_name);
            g_msg.copyfile_name = file_name.toStdString();
            g_msg.copyfile_id = this->sock->get_file_id(g_msg.copyfile_name);
            g_msg.copyfile_dir_id = g_msg.get_cur_id();
            g_msg.copyfile_status = PASTE_CUTFILE;
        }
    }
    reloadFile();
    renderFileList(file_list, dir_list);
}

// 双击进入文件夹
void NetDisk::on_file_list_cellDoubleClicked(int row, int column)
{
    if (row >= file_list.length() && column < 2) {
        // 进入其中
        QString dir_name = dir_list.at(row - file_list.length());
        this->sock->cd_dir(dir_name.toStdString());
        reloadFile();
        path = QString::fromStdString(g_msg.get_path());
        on_refresh_btn_clicked();
    }
}

void NetDisk::on_transfer_list_menu_triggered()
{
    g_msg.test_info();
    TransferListDialog transfer_dialog;
    this->hide();
    transfer_dialog.exec();
    this->show();
}

