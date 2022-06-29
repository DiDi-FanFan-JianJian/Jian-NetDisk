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

void TransferListDialog::my_timer_timeout()
{
    // 将所有文件的finished_size + 1
    for (auto &&it: g_msg.upload_file_list) {
        it.finished_size++;
    }
    for (auto &&it: g_msg.download_file_list) {
        it.finished_size++;
    }
    // 渲染文件列表
    renderFileList();
}

void TransferListDialog::renderFileList()
{
    // 使用QTableWidget控件显示文件上传列表
    ui->upload_file_list->clear();
    ui->upload_file_list->setColumnCount(4);
    ui->upload_file_list->setRowCount(g_msg.upload_file_list.size());
    ui->upload_file_list->setHorizontalHeaderLabels(QStringList() << "文件名" << "进度" << "状态" << "操作");
    for (unsigned i = 0; i < g_msg.upload_file_list.size(); i++) {
        // 文件名
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.upload_file_list[i].filename));
        ui->upload_file_list->setItem(i, 0, item);
        // 进度，使用QProgressBar控件显示进度
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(g_msg.upload_file_list[i].finished_size * 100 / g_msg.upload_file_list[i].file_size);
        ui->upload_file_list->setCellWidget(i, 1, bar);
        // 状态
        item = new QTableWidgetItem(QString::fromStdString(g_msg.upload_file_list[i].working ? "正在传输" : "未完成"));
        ui->upload_file_list->setItem(i, 2, item);
        // 控制暂停、继续按钮
        QPushButton *btn = new QPushButton();
        btn->setText(g_msg.upload_file_list[i].working ? "暂停" : "继续");
        ui->upload_file_list->setCellWidget(i, 3, btn);
    }

    // 使用QTableWidget控件显示文件下载列表
    ui->download_file_list->clear();
    ui->download_file_list->setColumnCount(4);
    ui->download_file_list->setRowCount(g_msg.download_file_list.size());
    ui->download_file_list->setHorizontalHeaderLabels(QStringList() << "文件名" << "进度" << "状态" << "操作");
    for (unsigned i = 0; i < g_msg.download_file_list.size(); i++) {
        // 文件名
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(g_msg.download_file_list[i].filename));
        ui->download_file_list->setItem(i, 0, item);
        // 进度，使用QProgressBar控件显示进度
        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(g_msg.download_file_list[i].finished_size * 100 / g_msg.download_file_list[i].file_size);
        ui->download_file_list->setCellWidget(i, 1, bar);
        // 状态
        item = new QTableWidgetItem(QString::fromStdString(g_msg.download_file_list[i].working ? "正在传输" : "未完成"));
        ui->download_file_list->setItem(i, 2, item);
        // 控制暂停、继续文字（如果正在传输，则显示暂停，否则显示继续）
        QPushButton *btn = new QPushButton();
        btn->setText(g_msg.download_file_list[i].working ? "暂停" : "继续");
        ui->download_file_list->setCellWidget(i, 3, btn);
    }
}

void TransferListDialog::on_upload_file_list_cellClicked(int row, int column)
{
    // 根据row获取文件信息
    int file_index = row;
    g_msg.upload_file_list[file_index];
    // 点击暂停、继续按钮
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
    // 根据row获取文件信息
    int file_index = row;
    g_msg.download_file_list[file_index];
    // 点击暂停、继续按钮
    if (column == 3) {
        if (g_msg.download_file_list[row].working) {
            g_msg.download_file_list[row].working = false;
        } else {
            g_msg.download_file_list[row].working = true;
        }
    }
}
