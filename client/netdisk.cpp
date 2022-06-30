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
    this->transfer_dialog = new TransferListDialog(this);
    this->transfer_dialog->hide();
    this->transfer_dialog->setWindowFlags(this->transfer_dialog->windowFlags() | Qt::Tool);

    g_msg.read_file_list();
    ui->setupUi(this);
    path = "root/";
    ui->now_path->setText(path);
    reloadFile();
}

NetDisk::~NetDisk()
{
    delete transfer_dialog;
    delete ui;
}

void NetDisk::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this, QStringLiteral("退出程序"), QStringLiteral("确认退出程序"), QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No) {
        event->ignore(); // 忽略退出信号，程序继续进行
    }
    else if(button==QMessageBox::Yes) {
        g_msg.write_file_list(); // 写入文件列表
        event->accept(); // 接受退出信号，程序退出
    }
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

// 大文件（具体的函数中的编码有个地方不是很确定）
void NetDisk::on_upload_file_clicked() {
    // 选择文件（限制只能选择文件）
    QString file_path = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"), ".", "All files(*.*)");
    if (file_path.isEmpty()) {
        // 取消选择或者关闭窗口
        return;
    }
    else if (checkSameName(getFileName(file_path))) {
        showMsg(QStringLiteral("文件名已存在"));
        return;
    }
    else {
        string path = file_path.toStdString();
        // showMsg(QString::fromStdString(path));
        this->sock->init_file_task(path);
    }
}

// 还没实现具体函数
void NetDisk::on_upload_dir_clicked() {
    // 选择文件夹（限制只能选择文件夹）
    QString dir_path = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件夹"), ".");
    if (dir_path.isEmpty()) {
        // 取消选择或者关闭窗口
        return;
    }
    else {
        QString dir_name = getFileName(dir_path);
        if (dir_name.isEmpty()) {
            // 应该是传了C盘的路径（C:/）
            showMsg(QStringLiteral("？？？"));
            return;
        }
        else if (checkSameName(dir_name)) {
            showMsg(QStringLiteral("文件夹名已存在"));
            return;
        }
        else {
            string path = dir_path.toStdString();
            this->sock->init_dir_task(path, g_msg.get_cur_id());

        }
    }
}

// 完成
void NetDisk::on_new_dir_clicked()
{
    // 弹框询问文件夹名称
    bool ok;
    QString dir_name = QInputDialog::getText(this, QStringLiteral("新建文件夹"), QStringLiteral("请输入文件夹名称"), QLineEdit::Normal, QString(""), &ok);
    if (ok) {
        if (dir_name.isEmpty()) {
            showMsg(QStringLiteral("输入不能为空"));
        }
        else if (dir_name.contains("/") || dir_name.contains("\\")) {
            showMsg(QStringLiteral("文件夹名称不能包含/或\\"));
        }
        else if (checkSameName(dir_name)) {
            showMsg(QStringLiteral("文件名已存在"));
        }
        else {
            showMsg(QStringLiteral("新建文件夹成功"));
            this->sock->create_dir(dir_name.toStdString());
            reloadFile();
            renderFileList(file_list, dir_list);
        }
    }
}

// 完成（后面可以考虑删除了复制/剪切的文件）
void NetDisk::on_paste_btn_clicked()
{
    // 判断有没有复制
    if (g_msg.copyfile_status == PASTE_NOFILE) {
        showMsg(QStringLiteral("请先复制文件"));
        return;
    }
    // 判断有没有重名
    if (checkSameName(QString::fromStdString(g_msg.copyfile_name))) {
        showMsg(QStringLiteral("文件名已存在"));
        return;
    }

    if (g_msg.copyfile_status == PASTE_COPYFILE) {
        // 复制文件
        this->sock->copy_file(g_msg.get_cur_id(), g_msg.copyfile_id, g_msg.copyfile_name);
    }
    else if (g_msg.copyfile_status == PASTE_COPYDIR) {
        // 复制文件夹
        if (checkSubDir()) {
            showMsg(QStringLiteral("不能复制到子文件夹"));
            return;
        }
        else {
            this->sock->copy_dir(g_msg.copyfile_id);
        }
    }
    else if (g_msg.copyfile_status == PASTE_CUTFILE) {
        // 剪切文件
        this->sock->move_file(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    else {
        // 剪切文件夹
        if (checkSubDir()) {
            showMsg(QStringLiteral("不能剪切到子文件夹"));
            return;
        }
        else {
            this->sock->move_dir(g_msg.copyfile_id, g_msg.copyfile_dir_id);
        }
    }
    reloadFile();
    renderFileList(file_list, dir_list);
}

// 渲染文件列表（大小还没完成）
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // 使用QTableWidget控件显示文件列表
    ui->file_list->clear();
    ui->file_list->setColumnCount(7);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    // 设置表头
    QStringList header;
    header.append(QStringLiteral("文件名"));
    header.append(QStringLiteral("文件类型"));
    header.append(QStringLiteral("下载"));
    header.append(QStringLiteral("删除"));
    header.append(QStringLiteral("重命名"));
    header.append(QStringLiteral("复制"));
    header.append(QStringLiteral("剪切"));
    ui->file_list->setHorizontalHeaderLabels(header);

    // 文件名和大小
    for (int i = 0; i < file_list.size(); i++) {
        // 文件名，给file_name设置icon，并且设置不可编辑
        QTableWidgetItem *file_name = new QTableWidgetItem(file_list.at(i));
        QIcon icon(":img/file_icon.png");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 0, file_name);
        // 文件大小
        QTableWidgetItem *file_size = new QTableWidgetItem(QStringLiteral("文件"));
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 1, file_size);
    }
    // 文件夹名
    for (int i = 0; i < dir_list.size(); i++) {
        // 文件名，给file_name设置icon
        QTableWidgetItem *file_name = new QTableWidgetItem(dir_list.at(i));
        QIcon icon(":img/dir_icon.jpg");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 0, file_name);
        // 文件大小
        QTableWidgetItem *file_size = new QTableWidgetItem(QStringLiteral("文件夹"));
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 1, file_size);
    }
    // 所有的图标
    for (int i = 0; i < file_list.size() + dir_list.size(); i++) {
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
}

// 显示消息
void NetDisk::showMsg(QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

// table中的按钮
// 下载：待完成
// 删除：处理删除文件夹（？）   删除文件没有连接sock
// 重命名：完成
// 复制：完成
// 剪切：完成
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
        QString download_path = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件夹"), ".");
        if (download_path.isEmpty()) {
            // 取消或者关闭对话框
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            QString dir_path = download_path + "/" + dir_name;
            int dir_id = g_msg.get_cur_id();
            int did = this->sock->get_dir_id(dir_name.toStdString());
            // this->sock->add_download_file(dir_name, dir_path);
            showMsg("下载文件夹" + dir_name + "到指定位置" + download_path);
        }
        else {
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
                int id = this->sock->get_dir_id(dir_name.toStdString());
                this->sock->delete_dir(id);
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
                int id = this->sock->get_file_id(file_name.toStdString());
                this->sock->delete_file(id, g_msg.get_cur_id());
            }
        }
    }
    else if (column == 4) {
        // 重命名
        bool ok;
        QString new_name = QInputDialog::getText(this, QStringLiteral("重命名"), QStringLiteral("请输入新的文件名"), QLineEdit::Normal, QString(""), &ok);
        if (!ok) {
            // 点击取消或者直接关闭
            return;
        }
        if (new_name.isEmpty()) {
            showMsg(QStringLiteral("输入不能为空"));
            return;
        }
        if (checkSameName(new_name)) {
            showMsg(QStringLiteral("文件名已存在"));
            return;
        }
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            if (dir_name == new_name) {
                showMsg(QStringLiteral("文件夹名不能与原名相同"));
                return;
            }
            showMsg(QStringLiteral("修改文件夹名称成功"));
            int id = this->sock->get_dir_id(dir_name.toStdString());
            this->sock->rename_dir(id, new_name.toStdString());
        }
        else {
            QString file_name = file_list.at(item_id);
            if (file_name == new_name) {
                showMsg(QStringLiteral("文件名不能与原名相同"));
                return;
            }
            showMsg(QStringLiteral("修改文件名称成功"));
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

// 双击进入文件夹：完成（编码处理通过）
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
    // g_msg.test_info();
    this->transfer_dialog->show();
}

bool NetDisk::checkSameName(QString name)
{
    for (int i = 0; i < file_list.length(); i++) {
        if (file_list[i] == name) {
            return true;
        }
    }
    for (int i = 0; i < dir_list.length(); i++) {
        if (dir_list[i] == name) {
            return true;
        }
    }
    return false;
}

bool NetDisk::checkSubDir()
{
    // 遍历cur_dir，其中如果有copyed_id，则说明是在子目录下，不能copy
    for (unsigned i = 0; i < g_msg.cur_dir.size(); i++) {
        if (g_msg.cur_dir[i] == g_msg.copyfile_id) {
            return true;
        }
    }
    return false;
}

QString NetDisk::getFileName(QString name)
{
    int i = name.lastIndexOf("/");
    return name.mid(i + 1);
}
