#include "netdisk.h"
#include "ui_netdisk.h"
#include "mysocket.h"

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
void NetDisk::testFile()
{
    file_list.append("test.txt");
    file_list.append("test.jpg");
    file_list.append("test.png");
    file_list.append("test.mp3");
    file_list.append("test.mp4");
    file_list.append("test.avi");
    file_list.append("test.doc");
    file_list.append("test.docx");
    file_list.append("test.xls");
    file_list.append("test.xlsx");
    dir_list.append("test");
    dir_list.append("test2");
    dir_list.append("test3");
    // 渲染文件列表
    renderFileList(file_list, dir_list);
    cout << g_msg.username << " " << g_msg.cur_dir << endl;
}


NetDisk::NetDisk(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetDisk)
{
    ui->setupUi(this);
    path = "root/";
    ui->now_path->setText(path);
    testFile();
}

NetDisk::~NetDisk()
{
    delete ui;
}

void NetDisk::on_refresh_btn_clicked() {
    showMsg("刷新列表");

    // 渲染文件列表
    ui->now_path->setText(path);
    renderFileList(file_list, dir_list);
}

void NetDisk::on_return_btn_clicked() {
    showMsg("返回上一级");

    // 渲染文件列表
    ui->now_path->setText(path);
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
    QString dir_name = QInputDialog::getText(this, "新建文件夹", "请输入文件夹名称");
    if (dir_name.isEmpty()) {
        showMsg("你自己关了，无事发生");
    }
    else {
        showMsg(dir_name);

        dir_list.append(dir_name);
        renderFileList(file_list, dir_list);
    }
}

// 以下同样缺少路径分析
void NetDisk::on_copy_dir_btn_clicked()
{
    // 弹框询问文件夹名称、目标文件夹
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("复制文件夹");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("源文件夹：");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("目标文件夹：");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("确定");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // 将QLabel、QLineEdit、QPushButton添加到QGridLayout中，并设置行数为0，列数为0，并设置行距为10，列距为10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // 当点击确定按钮时，获取文本内容，并显示到消息框中
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "文件夹复制到" + dst);

        if (dst == path) {

            dir_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("你自己关了，无事发生");
    }
}

void NetDisk::on_copy_file_btn_clicked()
{
    // 弹框询问文件名称、目标文件夹
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("复制文件");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("源文件：");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("目标文件夹：");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("确定");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // 将QLabel、QLineEdit、QPushButton添加到QGridLayout中，并设置行数为0，列数为0，并设置行距为10，列距为10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // 当点击确定按钮时，获取文本内容，并显示到消息框中
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "文件复制到" + dst);

        if (dst == path) {

            file_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("你自己关了，无事发生");
    }
}

void NetDisk::on_move_dir_btn_clicked()
{
    // 弹框询问文件夹名称、目标文件夹
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("移动文件夹");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("源文件夹：");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("目标文件夹：");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("确定");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // 将QLabel、QLineEdit、QPushButton添加到QGridLayout中，并设置行数为0，列数为0，并设置行距为10，列距为10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // 当点击确定按钮时，获取文本内容，并显示到消息框中
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "文件夹移动到" + dst);

        // 是否需要更新文件列表
        if (dst == path) {

            dir_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("你自己关了，无事发生");
    }
}

void NetDisk::on_move_file_btn_clicked()
{
    // 弹框询问文件(文件夹)名称、目标文件夹
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("移动文件");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("源文件：");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("目标文件夹：");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("确定");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // 将QLabel、QLineEdit、QPushButton添加到QGridLayout中，并设置行数为0，列数为0，并设置行距为10，列距为10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // 当点击确定按钮时，获取文本内容，并显示到消息框中
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "文件移动到" + dst);

        // 是否需要更新文件列表
        if (dst == path) {

            file_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("你自己关了，无事发生");
    }
}


// 渲染文件列表
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // 使用QTableWidget控件显示文件列表
    ui->file_list->clear();
    ui->file_list->setColumnCount(5);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    ui->file_list->setHorizontalHeaderLabels(QStringList() << "文件名" << "大小" << "下载" << "删除" << "重命名");
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
        // 下载图案
        QPixmap *pixmap;
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
        // 下载图案
        QPixmap *pixmap;
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
    if (column >= 2 ) {
        int item_id, is_dir;
        // 计算文件/文件夹的id
        if (row < file_list.length()) {
            item_id = row;
            is_dir = 0;
        }
        else {
            item_id = row - file_list.length();
            is_dir = 1;
        }

        if (column == 2) {
            // 下载文件/文件夹
            if (is_dir) {
                QString dir_name = dir_list.at(item_id);
                QString download_path = QFileDialog::getExistingDirectory(this, "选择文件夹", "D:/");

                if (download_path.isEmpty()) {
                    showMsg("你自己关了，无事发生");
                    return;
                }
                else {
                    showMsg("下载文件夹" + dir_name + "到指定位置" + download_path);
                }
            }
            else {
                QString file_name = file_list.at(item_id);
                QString download_path = QFileDialog::getExistingDirectory(this, "选择文件夹", "D:/");

                if (download_path.isEmpty()) {
                    showMsg("你自己关了，无事发生");
                    return;
                }
                else {
                    showMsg("下载文件" + file_name + "到指定位置" + download_path);
                }
            }
        }
        else if (column == 3) {
            // 删除文件/文件夹
            if (is_dir) {
                QString dir_name = dir_list.at(item_id);
                // 询问是否删除
                QMessageBox msgbox;
                msgbox.setText("是否删除文件夹" + dir_name);
                msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgbox.setDefaultButton(QMessageBox::No);
                int ret = msgbox.exec();
                if (ret == QMessageBox::Yes) {
                    // 删除文件夹
                    showMsg("删除文件夹" + dir_name);

                    ui->file_list->removeRow(row);
                    dir_list.removeAt(item_id);
                }
            }
            else {
                QString file_name = file_list.at(item_id);
                // 询问是否删除
                QMessageBox msgbox;
                msgbox.setText("是否删除文件" + file_name);
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
            QString file_name = file_list.at(item_id);
            // 询问修改后的文件名
            QString new_name = QInputDialog::getText(this, "重命名", "请输入新的文件名");
            if (new_name.isEmpty()) {
                showMsg("你自己关了，无事发生");
                return;
            }
            else {
                if (is_dir) {
                    showMsg("重命名文件夹" + file_name + "为" + new_name);

                    ui->file_list->item(row, 0)->setText(new_name);
                    dir_list.replace(item_id, new_name);
                }
                else {
                    showMsg("重命名文件" + file_name + "为" + new_name);

                    ui->file_list->item(row, 0)->setText(new_name);
                    file_list.replace(item_id, new_name);
                }
            }


        }
    }
}

// 双击进入文件夹
void NetDisk::on_file_list_cellDoubleClicked(int row, int column)
{
    if (row >= file_list.length() && column < 2) {
        // 进入其中
        QString dir_name = dir_list.at(row - file_list.length());
        showMsg("进入文件夹" + dir_name);

        path += dir_name + "/";
        on_refresh_btn_clicked();
    }
}

