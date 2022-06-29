#include "transferlistdialog.h"
#include "ui_transferlistdialog.h"

#include <QProgressBar>
#include <QTimer>
#include <QPushButton>

#include "global_msg.h"

extern Global_Msg g_msg;

TransferListDialog::TransferListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferListDialog)
{
    this->sock = new SJ::MySocket("1.15.144.212", 8000);
    ui->setupUi(this);
    // ��ʱ����ÿ��һ����Ⱦһ���ļ��б�
    my_timer = new QTimer(this);
    connect(my_timer, SIGNAL(timeout()), this, SLOT(my_timer_timeout()));
    my_timer->start(1000);
}

TransferListDialog::~TransferListDialog()
{
    // ֹͣ��ʱ��
    my_timer->stop();
    delete my_timer;
    delete ui;
}

void TransferListDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore(); // �����˳��źţ������������
}

void TransferListDialog::my_timer_timeout()
{
    // �������ļ���finished_size + 1
    if (g_msg.upload_file_list.size()) {
        auto& fir = g_msg.upload_file_list[0];
        auto md5 = g_msg.file_md5[fir.file_path];
        this->sock->sendBlock(fir.file_path);
        if (g_msg.cur_idx[md5] > g_msg.block_num[md5]) {
            g_msg.upload_file_list.erase(g_msg.upload_file_list.begin());
        }
        else {
            fir.finished_size = min(g_msg.cur_idx[g_msg.file_md5[fir.file_path]], g_msg.block_num[md5]);
        }
    }
    // ��Ⱦ�ļ��б�
    renderFileList();
}

void TransferListDialog::renderFileList()
{
    // ʹ��QTableWidget�ؼ���ʾ�ļ��ϴ��б�
    ui->upload_file_list->clear();
    ui->upload_file_list->setColumnCount(4);
    ui->upload_file_list->setRowCount(g_msg.upload_file_list.size());
    ui->upload_file_list->setHorizontalHeaderLabels(QStringList() << "File" << "Progress Bar" << "Status" << "Option");
    for (unsigned i = 0; i < g_msg.upload_file_list.size(); i++) {
        // �ļ���
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.upload_file_list[i].filename));
        ui->upload_file_list->setItem(i, 0, item);
        // ���ȣ�ʹ��QProgressBar�ؼ���ʾ����
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(g_msg.upload_file_list[i].finished_size * 100 / g_msg.upload_file_list[i].file_size);
        ui->upload_file_list->setCellWidget(i, 1, bar);
        // ״̬
        item = new QTableWidgetItem(QString::fromStdString(g_msg.upload_file_list[i].working ? "uploading" : "waiting"));
        ui->upload_file_list->setItem(i, 2, item);
        // ������ͣ��������ť
        QPushButton *btn = new QPushButton();
        btn->setText(g_msg.upload_file_list[i].working ? "pause" : "continue");
        ui->upload_file_list->setCellWidget(i, 3, btn);
    }

    // ʹ��QTableWidget�ؼ���ʾ�ļ������б�
    ui->download_file_list->clear();
    ui->download_file_list->setColumnCount(4);
    ui->download_file_list->setRowCount(g_msg.download_file_list.size());
    ui->download_file_list->setHorizontalHeaderLabels(QStringList() << "File" << "Progress Bar" << "Status" << "Option");
    for (unsigned i = 0; i < g_msg.download_file_list.size(); i++) {
        // �ļ���
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.download_file_list[i].filename));
        ui->download_file_list->setItem(i, 0, item);
        // ���ȣ�ʹ��QProgressBar�ؼ���ʾ����
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(g_msg.download_file_list[i].finished_size * 100 / g_msg.download_file_list[i].file_size);
        ui->download_file_list->setCellWidget(i, 1, bar);
        // ״̬
        item = new QTableWidgetItem(QString::fromStdString(g_msg.download_file_list[i].working ? "downloading" : "waiting"));
        ui->download_file_list->setItem(i, 2, item);
        // ������ͣ���������֣�������ڴ��䣬����ʾ��ͣ��������ʾ������
        QPushButton *btn = new QPushButton();
        btn->setText(g_msg.download_file_list[i].working ? "pause" : "continue");
        ui->download_file_list->setCellWidget(i, 3, btn);
    }
}

void TransferListDialog::on_upload_file_list_cellClicked(int row, int column)
{
    // ����row��ȡ�ļ���Ϣ
    int file_index = row;
    g_msg.upload_file_list[file_index];
    // �����ͣ��������ť
    if (column == 3) {
        if (g_msg.upload_file_list[row].working) {
            g_msg.upload_file_list[row].working = false;
        } else {
            g_msg.upload_file_list[row].working = true;
        }
    }
}

void TransferListDialog::on_download_file_list_cellClicked(int row, int column)
{
    // ����row��ȡ�ļ���Ϣ
    int file_index = row;
    g_msg.download_file_list[file_index];
    // �����ͣ��������ť
    if (column == 3) {
        if (g_msg.download_file_list[row].working) {
            g_msg.download_file_list[row].working = false;
        } else {
            g_msg.download_file_list[row].working = true;
        }
    }
}
