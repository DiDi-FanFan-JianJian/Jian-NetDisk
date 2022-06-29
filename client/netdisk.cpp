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
    button=QMessageBox::question(this,tr("�˳�����"),QString(tr("ȷ���˳�����")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No) {
        event->ignore(); // �����˳��źţ������������
    }
    else if(button==QMessageBox::Yes) {
        event->accept(); // �����˳��źţ������˳�
    }
    g_msg.write_file_list();
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

// ȱ���ļ�·������
void NetDisk::on_upload_file_clicked() {
    // getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, Options options)
    //                      ����ָ��                ����              Ĭ�ϴ��ļ���         �ļ����˹���          Ĭ�ϵ��ļ����˹���         ���ԣ���ʾ�ļ��еȣ�
    //  filter��"images(*.png *jpeg *bmp);;text files(*.txt *.doc *.docx);;video files(*.avi *.mp4 *.wmv);;All files(*.*)"

    // ѡ���ļ�������ֻ��ѡ���ļ���
    QString file_path = QFileDialog::getOpenFileName(this, "ѡ���ļ�", ".", "All files(*.*)");
    if (file_path.isEmpty()) {
        showMsg("���Լ����ˣ����·���");
        return;
    }
    else {
        showMsg("ѡ���ļ���" + file_path);

        file_list.append(file_path);
        renderFileList(file_list, dir_list);
    }
}

// ȱ���ļ�·������
void NetDisk::on_upload_dir_clicked() {
    // ѡ���ļ��У�����ֻ��ѡ���ļ��У�
    QString dir_name = QFileDialog::getExistingDirectory(this, "ѡ���ļ���", "D:/");
    if (dir_name.isEmpty()) {
        showMsg("���Լ����ˣ����·���");
    }
    else {
        showMsg(dir_name);

        dir_list.append(dir_name);
        renderFileList(file_list, dir_list);
    }
}

void NetDisk::on_new_dir_clicked()
{
    // ����ѯ���ļ�������
    bool ok;
    QString dir_name = QInputDialog::getText(this, QStringLiteral("�½��ļ���"), QStringLiteral("�������ļ�������"), QLineEdit::Normal, QString(""), &ok);
    if (ok) {
        if (dir_name.isEmpty()) {
            showMsg(QStringLiteral("���벻��Ϊ��"));
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
    // �ж���û�и���
    if (g_msg.copyfile_status == PASTE_NOFILE) {
        showMsg("���ȸ����ļ�");
        return;
    }

    QString dir_id = QString(g_msg.copyfile_dir_id);
    if (g_msg.copyfile_status == PASTE_COPYFILE) {
        // �����ļ�
        this->sock->copy_file(g_msg.get_cur_id(), g_msg.copyfile_id, g_msg.copyfile_name);
    }
    else if (g_msg.copyfile_status == PASTE_COPYDIR) {
        // �����ļ���
        this->sock->copy_dir(g_msg.copyfile_id);
    }
    else if (g_msg.copyfile_status == PASTE_CUTFILE) {
        // �����ļ�
        this->sock->move_file(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    else {
        // �����ļ���
        this->sock->move_dir(g_msg.copyfile_id, g_msg.copyfile_dir_id);
    }
    reloadFile();
    renderFileList(file_list, dir_list);
}

// ��Ⱦ�ļ��б�
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // ʹ��QTableWidget�ؼ���ʾ�ļ��б�
    ui->file_list->clear();
    ui->file_list->setColumnCount(7);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    ui->file_list->setHorizontalHeaderLabels(QStringList() << "�ļ���" << "��С" << "����" << "ɾ��" << "������" << "����" << "����");
    for (int i = 0; i < file_list.size(); i++) {
        // �ļ�������file_name����icon���������ò��ɱ༭
        QTableWidgetItem *file_name = new QTableWidgetItem(file_list.at(i));
        QIcon icon(":img/file_icon.png");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 0, file_name);
        // �ļ���С
        QTableWidgetItem *file_size = new QTableWidgetItem("1.0KB");
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i, 1, file_size);

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
    for (int i = 0; i < dir_list.size(); i++) {
        // �ļ�������file_name����icon
        QTableWidgetItem *file_name = new QTableWidgetItem(dir_list.at(i));
        QIcon icon(":img/dir_icon.jpg");
        file_name->setIcon(icon);
        file_name->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 0, file_name);
        // �ļ���С
        QTableWidgetItem *file_size = new QTableWidgetItem("�ļ���");
        file_size->setFlags(Qt::ItemIsEnabled);
        ui->file_list->setItem(i + file_list.size(), 1, file_size);
        QPixmap *pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/download.png");
        QLabel *download_label = new QLabel();
        QPixmap download_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        download_label->setPixmap(download_img);
        download_label->setAlignment(Qt::AlignCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 2, download_label);
        delete pixmap;
        // ɾ��ͼ��
        pixmap = new QPixmap(":img/delete.png");
        QLabel *delete_label = new QLabel();
        QPixmap delete_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        delete_label->setPixmap(delete_img);
        delete_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 3, delete_label);
        delete pixmap;
        // ������ͼ��
        pixmap = new QPixmap(":img/rename.png");
        QLabel *rename_label = new QLabel();
        QPixmap rename_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        rename_label->setPixmap(rename_img);
        rename_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 4, rename_label);
        delete pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/copy.png");
        QLabel *copy_label = new QLabel();
        QPixmap copy_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        copy_label->setPixmap(copy_img);
        copy_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 5, copy_label);
        delete pixmap;
        // ����ͼ��
        pixmap = new QPixmap(":img/cut.png");
        QLabel *cut_label = new QLabel();
        QPixmap cut_img = pixmap->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cut_label->setPixmap(cut_img);
        cut_label->setAlignment(Qt::AlignHCenter);
        ui->file_list->setCellWidget(i + file_list.size(), 6, cut_label);
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
        QString download_path = QFileDialog::getExistingDirectory(this, "ѡ���ļ���", "C:/");
        if (download_path.isEmpty()) {
            // ȡ�����߹رնԻ���
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            showMsg("�����ļ���" + dir_name + "��ָ��λ��" + download_path);
        }
        else {
            QString file_name = file_list.at(item_id);
            showMsg("�����ļ�" + file_name + "��ָ��λ��" + download_path);
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
                showMsg("ɾ���ļ���" + dir_name);
                int id = this->sock->get_dir_id(dir_name.toStdString());
                this->sock->delete_dir(id);
                cout << dir_name.toStdString() << " " << id << endl;
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
                showMsg("ɾ���ļ�" + file_name);

                ui->file_list->removeRow(row);
                file_list.removeAt(item_id);
            }
        }
    }
    else if (column == 4) {
        // ������
        bool ok;
        QString new_name = QInputDialog::getText(this, "������", "�������µ��ļ���", QLineEdit::Normal, QString(""), &ok);
        if (!ok) {
            // ���ȡ������ֱ�ӹر�
            return;
        }
        if (new_name.isEmpty()) {
            showMsg(QStringLiteral("���벻��Ϊ��"));
            return;
        }

        if (is_dir) {
            QString dir_name = dir_list.at(item_id);
            showMsg("�������ļ���" + dir_name + "Ϊ" + new_name);
            int id = this->sock->get_dir_id(dir_name.toStdString());
            this->sock->rename_dir(id, new_name.toStdString());
        }
        else {
            QString file_name = file_list.at(item_id);
            showMsg("�������ļ�" + file_name + "Ϊ" + new_name);
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

// ˫�������ļ���
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
    g_msg.test_info();
    TransferListDialog transfer_dialog;
    this->hide();
    transfer_dialog.exec();
    this->show();
}

