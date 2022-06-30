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
    // 定时器，每隔一秒渲染一次文件列表
    my_timer = new QTimer(this);
    connect(my_timer, SIGNAL(timeout()), this, SLOT(my_timer_timeout()));
    my_timer->start(1000);
}

TransferListDialog::~TransferListDialog()
{
    // 停止定时器
    my_timer->stop();
    delete my_timer;
    delete ui;
}

void TransferListDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore(); // 忽略退出信号，程序继续进行
}

void TransferListDialog::my_timer_timeout()
{
    // 将所有文件的finished_size + 1
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
    this->sock->recvBlock();
    // 渲染文件列表
    renderFileList();
}

void TransferListDialog::renderFileList()
{
    // 使用QTableWidget控件显示文件上传列表
    ui->upload_file_list->clear();
    ui->upload_file_list->setColumnCount(4);
    ui->upload_file_list->setRowCount(g_msg.upload_file_list.size());
    ui->upload_file_list->setHorizontalHeaderLabels(QStringList() << "File" << "Progress Bar" << "Status" << "Option");
    for (unsigned i = 0; i < g_msg.upload_file_list.size(); i++) {
        // 文件名
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.upload_file_list[i].filename));
        ui->upload_file_list->setItem(i, 0, item);
        // 进度，使用QProgressBar控件显示进度
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        if (g_msg.upload_file_list[i].file_size) {
            bar->setValue(g_msg.upload_file_list[i].finished_size * 100 / g_msg.upload_file_list[i].file_size);
        }
        else {
            bar->setValue(100);
        }
        ui->upload_file_list->setCellWidget(i, 1, bar);
        // 状态
        if (g_msg.upload_file_list[i].working == 0) {
            item = new QTableWidgetItem(QString::fromStdString("stop"));
            ui->upload_file_list->setItem(i, 2, item);
        }
        else if (g_msg.upload_file_list[i].working == 1) {
            item = new QTableWidgetItem(QString::fromStdString("uploading"));
            ui->upload_file_list->setItem(i, 2, item);
        }
        else {
            item = new QTableWidgetItem(QString::fromStdString("waiting"));
            ui->upload_file_list->setItem(i, 2, item);
        }
        // 控制暂停、继续按钮
        item = new QTableWidgetItem(QString::fromStdString((g_msg.upload_file_list[i].working == 0) ? "start" : "stop"));
        ui->upload_file_list->setItem(i, 3, item);
    }

    // 使用QTableWidget控件显示文件下载列表
    ui->download_file_list->clear();
    ui->download_file_list->setColumnCount(4);
    ui->download_file_list->setRowCount(g_msg.download_file_list.size());
    ui->download_file_list->setHorizontalHeaderLabels(QStringList() << "File" << "Progress Bar" << "Status" << "Option");
    for (unsigned i = 0; i < g_msg.download_file_list.size(); i++) {
        // 文件名
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.download_file_list[i].filename));
        ui->download_file_list->setItem(i, 0, item);
        // 进度，使用QProgressBar控件显示进度
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        if (g_msg.download_file_list[i].file_size) {
            bar->setValue(g_msg.download_file_list[i].finished_size * 100 / g_msg.download_file_list[i].file_size);
        }
        else {
            bar->setValue(100);
        }
        ui->download_file_list->setCellWidget(i, 1, bar);
        // 状态
        if (g_msg.download_file_list[i].working == 0) {
            item = new QTableWidgetItem(QString::fromStdString("stop"));
            ui->download_file_list->setItem(i, 2, item);
        }
        else if (g_msg.download_file_list[i].working == 1) {
            item = new QTableWidgetItem(QString::fromStdString("downloading"));
            ui->download_file_list->setItem(i, 2, item);
        }
        else {
            item = new QTableWidgetItem(QString::fromStdString("waiting"));
            ui->download_file_list->setItem(i, 2, item);
        }
        // 控制暂停、继续文字（如果正在传输，则显示暂停，否则显示继续）
        item = new QTableWidgetItem(QString::fromStdString((g_msg.download_file_list[i].working == 0) ? "start" : "stop"));
        ui->download_file_list->setItem(i, 3, item);
    }
}

void TransferListDialog::on_upload_file_list_cellClicked(int row, int column)
{
    // 根据row获取文件信息
    int file_index = row;
    g_msg.upload_file_list[file_index];
    // 点击暂停、继续按钮
    if (column == 3) {
        if (g_msg.upload_file_list[row].working == 0) {
            g_msg.upload_file_list[row].working = 1;
        } else {
            g_msg.upload_file_list[row].working = 0;
        }
    }
}

void TransferListDialog::on_download_file_list_cellClicked(int row, int column)
{
    // 根据row获取文件信息
    int file_index = row;
    g_msg.download_file_list[file_index];
    // 点击暂停、继续按钮
    if (column == 3) {
        if (g_msg.download_file_list[row].working == 0) {
            g_msg.download_file_list[row].working = 2;
        } else {
            g_msg.download_file_list[row].working = 0;
        }
    }
}
