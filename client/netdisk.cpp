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

// ��Ӳ����ļ�/�ļ���
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
    // ��Ⱦ�ļ��б�
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
    button=QMessageBox::question(this, QStringLiteral("�˳�����"), QStringLiteral("ȷ���˳�����"), QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No) {
        event->ignore(); // �����˳��źţ������������
    }
    else if(button==QMessageBox::Yes) {
        g_msg.write_file_list(); // д���ļ��б�
        event->accept(); // �����˳��źţ������˳�
    }
}

void NetDisk::on_refresh_btn_clicked() {
    // ��Ⱦ�ļ��б�
    ui->now_path->setText(path);
    reloadFile();
    renderFileList(file_list, dir_list);
}

void NetDisk::on_return_btn_clicked() {
    g_msg.go_back();
    path = QString::fromStdString(g_msg.get_path());
    // ��Ⱦ�ļ��б�
    ui->now_path->setText(path);
    reloadFile();
    renderFileList(file_list, dir_list);
}

// ���ļ�������ĺ����еı����и��ط����Ǻ�ȷ����
void NetDisk::on_upload_file_clicked() {
    // ѡ���ļ�������ֻ��ѡ���ļ���
    QString file_path = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ���ļ�"), ".", "All files(*.*)");
    if (file_path.isEmpty()) {
        // ȡ��ѡ����߹رմ���
        return;
    }
    else if (checkSameName(getFileName(file_path))) {
        showMsg(QStringLiteral("�ļ����Ѵ���"));
        return;
    }
    else {
        string path = file_path.toStdString();
        // showMsg(QString::fromStdString(path));
        this->sock->init_file_task(path);
    }
}

// ��ûʵ�־��庯��
void NetDisk::on_upload_dir_clicked() {
    // ѡ���ļ��У�����ֻ��ѡ���ļ��У�
    QString dir_path = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ���ļ���"), ".");
    if (dir_path.isEmpty()) {
        // ȡ��ѡ����߹رմ���
        return;
    }
    else {
        QString dir_name = getFileName(dir_path);
        if (dir_name.isEmpty()) {
            // Ӧ���Ǵ���C�̵�·����C:/��
            showMsg(QStringLiteral("������"));
            return;
        }
        else if (checkSameName(dir_name)) {
            showMsg(QStringLiteral("�ļ������Ѵ���"));
            return;
        }
        else {
            string path = dir_path.toStdString();
            this->sock->init_dir_task(path, g_msg.get_cur_id());

        }
    }
}

// ���
void NetDisk::on_new_dir_clicked()
{
    // ����ѯ���ļ�������
    bool ok;
    QString dir_name = QInputDialog::getText(this, QStringLiteral("�½��ļ���"), QStringLiteral("�������ļ�������"), QLineEdit::Normal, QString(""), &ok);
    if (ok) {
        if (dir_name.isEmpty()) {
            showMsg(QStringLiteral("���벻��Ϊ��"));
        }
        else if (dir_name.contains("/") || dir_name.contains("\\")) {
            showMsg(QStringLiteral("�ļ������Ʋ��ܰ���/��\\"));
        }
        else if (checkSameName(dir_name)) {
            showMsg(QStringLiteral("�ļ����Ѵ���"));
        }
        else {
            showMsg(QStringLiteral("�½��ļ��гɹ�"));
            this->sock->create_dir(dir_name.toStdString());
            reloadFile();
            renderFileList(file_list, dir_list);
        }
    }
}

// ��ɣ�������Կ���ɾ���˸���/���е��ļ���
void NetDisk::on_paste_btn_clicked()
{
    // �ж���û�и���
    if (g_msg.copyfile_status == PASTE_NOFILE) {
        showMsg(QStringLiteral("���ȸ����ļ�"));
        return;
    }
    // �ж���û������
    if (checkSameName(QString::fromStdString(g_msg.copyfile_name))) {
        showMsg(QStringLiteral("�ļ����Ѵ���"));
        return;
    }

    if (g_msg.copyfile_status == PASTE_COPYFILE) {
        // �����ļ�
        this->sock->copy_file(g_msg.get_cur_id(), g_msg.copyfile_id, g_msg.copyfile_name);
    }
    else if (g_msg.copyfile_status == PASTE_COPYDIR) {
        // �����ļ���
        if (checkSubDir()) {
            showMsg(QStringLiteral("���ܸ��Ƶ����ļ���"));
            return;
        }
        else {
            this->sock->copy_dir(g_msg.copyfile_id);
        }
    }
    else if (g_msg.copyfile_status == PASTE_CUTFILE) {
        // �����ļ�
        this->sock->move_file(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    else {
        // �����ļ���
        if (checkSubDir()) {
            showMsg(QStringLiteral("���ܼ��е����ļ���"));
            return;
        }
        else {
            this->sock->move_dir(g_msg.copyfile_id, g_msg.copyfile_dir_id);
        }
    }
    reloadFile();
    renderFileList(file_list, dir_list);
}

// ��Ⱦ�ļ��б���С��û��ɣ�
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // ʹ��QTableWidget�ؼ���ʾ�ļ��б�
    ui->file_list->clear();
    ui->file_list->setColumnCount(7);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    // ���ñ�ͷ
    QStringList header;
    header.append(QStringLiteral("�ļ���"));
    header.append(QStringLiteral("�ļ�����"));
    header.append(QStringLiteral("����"));
    header.append(QStringLiteral("ɾ��"));
    header.append(QStringLiteral("������"));
    header.append(QStringLiteral("����"));
    header.append(QStringLiteral("����"));
    ui->file_list->setHorizontalHeaderLabels(header);

    // �ļ����ʹ�С
    for (int i = 0; i < file_list.size(); i++) {
        // �ļ�������file_name����icon���������ò��ɱ༭
        QTableWidgetItem *file_name = new QTableWidgetItem(file_list.at(i));
        QIcon icon(":img/file_icon.png");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 0, file_name);
        // �ļ���С
        QTableWidgetItem *file_size = new QTableWidgetItem(QStringLiteral("�ļ�"));
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 1, file_size);
    }
    // �ļ�����
    for (int i = 0; i < dir_list.size(); i++) {
        // �ļ�������file_name����icon
        QTableWidgetItem *file_name = new QTableWidgetItem(dir_list.at(i));
        QIcon icon(":img/dir_icon.jpg");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 0, file_name);
        // �ļ���С
        QTableWidgetItem *file_size = new QTableWidgetItem(QStringLiteral("�ļ���"));
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 1, file_size);
    }
    // ���е�ͼ��
    for (int i = 0; i < file_list.size() + dir_list.size(); i++) {
        QPixmap *pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/download.png");
        QLabel *download_label = new QLabel();
        QPixmap download_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        download_label->setPixmap(download_img);
        download_label->setAlignment(Qt::AlignCenter);
        ui->file_list->setCellWidget(i, 2, download_label);
        delete pixmap;
        // ɾ��ͼ��
        pixmap = new QPixmap(":img/delete.png");
        QLabel *delete_label = new QLabel();
        QPixmap delete_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        delete_label->setPixmap(delete_img);
        delete_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 3, delete_label);
        delete pixmap;
        // ������ͼ��
        pixmap = new QPixmap(":img/rename.png");
        QLabel *rename_label = new QLabel();
        QPixmap rename_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        rename_label->setPixmap(rename_img);
        rename_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 4, rename_label);
        delete pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/copy.png");
        QLabel *copy_label = new QLabel();
        QPixmap copy_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        copy_label->setPixmap(copy_img);
        copy_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 5, copy_label);
        delete pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/cut.png");
        QLabel *cut_label = new QLabel();
        QPixmap cut_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cut_label->setPixmap(cut_img);
        cut_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i, 6, cut_label);
        delete pixmap;
    }
}

// ��ʾ��Ϣ
void NetDisk::showMsg(QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

// table�еİ�ť
// ���أ������
// ɾ��������ɾ���ļ��У�����   ɾ���ļ�û������sock
// �����������
// ���ƣ����
// ���У����
void NetDisk::on_file_list_cellClicked(int row, int column)
{
    if (column < 2 ) {
        // �����һ�С��ڶ�����Ч��
        return;
    }

    // �����ļ�/�ļ��е�id
    int item_id, is_dir;
    if (row < file_list.length()) {
        item_id = row;
        is_dir = 0;
    }
    else {
        item_id = row - file_list.length();
        is_dir = 1;
    }

    // ִ�ж�Ӧ����
    if (column == 2) {
        // �����ļ�/�ļ���
        QString download_path = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ���ļ���"), ".");
        if (download_path.isEmpty()) {
            // ȡ�����߹رնԻ���
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            QString dir_path = download_path + "/" + dir_name;
            int dir_id = g_msg.get_cur_id();
            int did = this->sock->get_dir_id(dir_name.toStdString());
            // this->sock->add_download_file(dir_name, dir_path);
            showMsg("�����ļ���" + dir_name + "��ָ��λ��" + download_path);
        }
        else {
        }
    }
    else if (column == 3) {
        // ɾ���ļ�/�ļ���
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            // ѯ���Ƿ�ɾ��
            QMessageBox msgbox;
            msgbox.setText("Delete " + dir_name + "?");
            msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgbox.setDefaultButton(QMessageBox::No);
            int ret = msgbox.exec();
            if (ret == QMessageBox::Yes) {
                // ɾ���ļ���
                int id = this->sock->get_dir_id(dir_name.toStdString());
                this->sock->delete_dir(id);
            }
        }
        else {
            QString file_name = file_list.at(item_id);
            // ѯ���Ƿ�ɾ��
            QMessageBox msgbox;
            msgbox.setText("Delete " + file_name + "?");
            msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgbox.setDefaultButton(QMessageBox::No);
            int ret = msgbox.exec();
            if (ret == QMessageBox::Yes) {
                // ɾ���ļ�
                int id = this->sock->get_file_id(file_name.toStdString());
                this->sock->delete_file(id, g_msg.get_cur_id());
            }
        }
    }
    else if (column == 4) {
        // ������
        bool ok;
        QString new_name = QInputDialog::getText(this, QStringLiteral("������"), QStringLiteral("�������µ��ļ���"), QLineEdit::Normal, QString(""), &ok);
        if (!ok) {
            // ���ȡ������ֱ�ӹر�
            return;
        }
        if (new_name.isEmpty()) {
            showMsg(QStringLiteral("���벻��Ϊ��"));
            return;
        }
        if (checkSameName(new_name)) {
            showMsg(QStringLiteral("�ļ����Ѵ���"));
            return;
        }
        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            if (dir_name == new_name) {
                showMsg(QStringLiteral("�ļ�����������ԭ����ͬ"));
                return;
            }
            showMsg(QStringLiteral("�޸��ļ������Ƴɹ�"));
            int id = this->sock->get_dir_id(dir_name.toStdString());
            this->sock->rename_dir(id, new_name.toStdString());
        }
        else {
            QString file_name = file_list.at(item_id);
            if (file_name == new_name) {
                showMsg(QStringLiteral("�ļ���������ԭ����ͬ"));
                return;
            }
            showMsg(QStringLiteral("�޸��ļ����Ƴɹ�"));
            int id = this->sock->get_file_id(file_name.toStdString());
            this->sock->rename_file(id, new_name.toStdString());
        }
    }
    else if (column == 5) {
        // ����
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
        // ����
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

// ˫�������ļ��У���ɣ����봦��ͨ����
void NetDisk::on_file_list_cellDoubleClicked(int row, int column)
{
    if (row >= file_list.length() && column < 2) {
        // ��������
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
    // ����cur_dir�����������copyed_id����˵��������Ŀ¼�£�����copy
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
