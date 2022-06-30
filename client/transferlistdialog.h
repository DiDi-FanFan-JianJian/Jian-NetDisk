#ifndef TRANSFERLISTDIALOG_H
#define TRANSFERLISTDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QCloseEvent>

#include "mysocket.h"

namespace Ui {
class TransferListDialog;
}

class TransferListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferListDialog(QWidget *parent = nullptr);
    ~TransferListDialog();

    void renderFileList();    // 渲染所有的传输中的文件

private slots:
    void my_timer_timeout();

    void on_upload_file_list_cellClicked(int row, int column);

    void on_download_file_list_cellClicked(int row, int column);

protected:
     void closeEvent(QCloseEvent *event);

private:
    SJ::MySocket* sock;
    Ui::TransferListDialog *ui;
    // 定时器，每隔一秒渲染一次文件列表
    QTimer *my_timer;
};

#endif // TRANSFERLISTDIALOG_H
