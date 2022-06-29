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

// ��Ӳ����ļ�/�ļ���
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
    // ��Ⱦ�ļ��б�
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
    showMsg("ˢ���б�");

    // ��Ⱦ�ļ��б�
    ui->now_path->setText(path);
    renderFileList(file_list, dir_list);
}

void NetDisk::on_return_btn_clicked() {
    showMsg("������һ��");

    // ��Ⱦ�ļ��б�
    ui->now_path->setText(path);
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
    QString dir_name = QInputDialog::getText(this, "�½��ļ���", "�������ļ�������");
    if (dir_name.isEmpty()) {
        showMsg("���Լ����ˣ����·���");
    }
    else {
        showMsg(dir_name);

        dir_list.append(dir_name);
        renderFileList(file_list, dir_list);
    }
}

// ����ͬ��ȱ��·������
void NetDisk::on_copy_dir_btn_clicked()
{
    // ����ѯ���ļ������ơ�Ŀ���ļ���
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("�����ļ���");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("Դ�ļ��У�");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("Ŀ���ļ��У�");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("ȷ��");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // ��QLabel��QLineEdit��QPushButton��ӵ�QGridLayout�У�����������Ϊ0������Ϊ0���������о�Ϊ10���о�Ϊ10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // �����ȷ����ťʱ����ȡ�ı����ݣ�����ʾ����Ϣ����
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "�ļ��и��Ƶ�" + dst);

        if (dst == path) {

            dir_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("���Լ����ˣ����·���");
    }
}

void NetDisk::on_copy_file_btn_clicked()
{
    // ����ѯ���ļ����ơ�Ŀ���ļ���
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("�����ļ�");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("Դ�ļ���");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("Ŀ���ļ��У�");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("ȷ��");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // ��QLabel��QLineEdit��QPushButton��ӵ�QGridLayout�У�����������Ϊ0������Ϊ0���������о�Ϊ10���о�Ϊ10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // �����ȷ����ťʱ����ȡ�ı����ݣ�����ʾ����Ϣ����
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "�ļ����Ƶ�" + dst);

        if (dst == path) {

            file_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("���Լ����ˣ����·���");
    }
}

void NetDisk::on_move_dir_btn_clicked()
{
    // ����ѯ���ļ������ơ�Ŀ���ļ���
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("�ƶ��ļ���");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("Դ�ļ��У�");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("Ŀ���ļ��У�");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("ȷ��");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // ��QLabel��QLineEdit��QPushButton��ӵ�QGridLayout�У�����������Ϊ0������Ϊ0���������о�Ϊ10���о�Ϊ10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // �����ȷ����ťʱ����ȡ�ı����ݣ�����ʾ����Ϣ����
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "�ļ����ƶ���" + dst);

        // �Ƿ���Ҫ�����ļ��б�
        if (dst == path) {

            dir_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("���Լ����ˣ����·���");
    }
}

void NetDisk::on_move_file_btn_clicked()
{
    // ����ѯ���ļ�(�ļ���)���ơ�Ŀ���ļ���
    QString src, dst;
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("�ƶ��ļ�");
    QGridLayout *layout = new QGridLayout(dialog);
    layout->setSpacing(10);
    QLabel *label1 = new QLabel(dialog);
    label1->setText("Դ�ļ���");
    QLineEdit *lineEdit1 = new QLineEdit(dialog);
    lineEdit1->setText("");
    QLabel *label2 = new QLabel(dialog);
    label2->setText("Ŀ���ļ��У�");
    QLineEdit *lineEdit2 = new QLineEdit(dialog);
    lineEdit2->setText("");
    QPushButton *btn = new QPushButton(dialog);
    btn->setText("ȷ��");
    connect(btn, &QPushButton::clicked, [=]() { dialog->accept(); });

    // ��QLabel��QLineEdit��QPushButton��ӵ�QGridLayout�У�����������Ϊ0������Ϊ0���������о�Ϊ10���о�Ϊ10
    layout->addWidget(label1, 0, 0);
    layout->addWidget(lineEdit1, 0, 1);
    layout->addWidget(label2, 1, 0);
    layout->addWidget(lineEdit2, 1, 1);
    layout->addWidget(btn, 2, 0, 1, 2);
    dialog->setLayout(layout);
    dialog->show();

    // �����ȷ����ťʱ����ȡ�ı����ݣ�����ʾ����Ϣ����
    if (dialog->exec() == QDialog::Accepted) {
        src = lineEdit1->text();
        dst = lineEdit2->text();
        showMsg(src + "�ļ��ƶ���" + dst);

        // �Ƿ���Ҫ�����ļ��б�
        if (dst == path) {

            file_list.append(src);
            renderFileList(file_list, dir_list);
        }
    }
    else {
        showMsg("���Լ����ˣ����·���");
    }
}


// ��Ⱦ�ļ��б�
void NetDisk::renderFileList(QStringList file_list, QStringList dir_list) {
    // ʹ��QTableWidget�ؼ���ʾ�ļ��б�
    ui->file_list->clear();
    ui->file_list->setColumnCount(5);
    ui->file_list->setRowCount(file_list.size() + dir_list.size());
    ui->file_list->setHorizontalHeaderLabels(QStringList() << "�ļ���" << "��С" << "����" << "ɾ��" << "������");
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
        // ����ͼ��
        QPixmap *pixmap;
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
        // ����ͼ��
        QPixmap *pixmap;
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
    if (column >= 2 ) {
        int item_id, is_dir;
        // �����ļ�/�ļ��е�id
        if (row < file_list.length()) {
            item_id = row;
            is_dir = 0;
        }
        else {
            item_id = row - file_list.length();
            is_dir = 1;
        }

        if (column == 2) {
            // �����ļ�/�ļ���
            if (is_dir) {
                QString dir_name = dir_list.at(item_id);
                QString download_path = QFileDialog::getExistingDirectory(this, "ѡ���ļ���", "D:/");

                if (download_path.isEmpty()) {
                    showMsg("���Լ����ˣ����·���");
                    return;
                }
                else {
                    showMsg("�����ļ���" + dir_name + "��ָ��λ��" + download_path);
                }
            }
            else {
                QString file_name = file_list.at(item_id);
                QString download_path = QFileDialog::getExistingDirectory(this, "ѡ���ļ���", "D:/");

                if (download_path.isEmpty()) {
                    showMsg("���Լ����ˣ����·���");
                    return;
                }
                else {
                    showMsg("�����ļ�" + file_name + "��ָ��λ��" + download_path);
                }
            }
        }
        else if (column == 3) {
            // ɾ���ļ�/�ļ���
            if (is_dir) {
                QString dir_name = dir_list.at(item_id);
                // ѯ���Ƿ�ɾ��
                QMessageBox msgbox;
                msgbox.setText("�Ƿ�ɾ���ļ���" + dir_name);
                msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgbox.setDefaultButton(QMessageBox::No);
                int ret = msgbox.exec();
                if (ret == QMessageBox::Yes) {
                    // ɾ���ļ���
                    showMsg("ɾ���ļ���" + dir_name);

                    ui->file_list->removeRow(row);
                    dir_list.removeAt(item_id);
                }
            }
            else {
                QString file_name = file_list.at(item_id);
                // ѯ���Ƿ�ɾ��
                QMessageBox msgbox;
                msgbox.setText("�Ƿ�ɾ���ļ�" + file_name);
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
            QString file_name = file_list.at(item_id);
            // ѯ���޸ĺ���ļ���
            QString new_name = QInputDialog::getText(this, "������", "�������µ��ļ���");
            if (new_name.isEmpty()) {
                showMsg("���Լ����ˣ����·���");
                return;
            }
            else {
                if (is_dir) {
                    showMsg("�������ļ���" + file_name + "Ϊ" + new_name);

                    ui->file_list->item(row, 0)->setText(new_name);
                    dir_list.replace(item_id, new_name);
                }
                else {
                    showMsg("�������ļ�" + file_name + "Ϊ" + new_name);

                    ui->file_list->item(row, 0)->setText(new_name);
                    file_list.replace(item_id, new_name);
                }
            }


        }
    }
}

// ˫�������ļ���
void NetDisk::on_file_list_cellDoubleClicked(int row, int column)
{
    if (row >= file_list.length() && column < 2) {
        // ��������
        QString dir_name = dir_list.at(row - file_list.length());
        showMsg("�����ļ���" + dir_name);

        path += dir_name + "/";
        on_refresh_btn_clicked();
    }
}

