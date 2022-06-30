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

    void renderFileList();    // ��Ⱦ���еĴ����е��ļ�

private slots:
    void my_timer_timeout();

    void on_upload_file_list_cellClicked(int row, int column);

    void on_download_file_list_cellClicked(int row, int column);

protected:
     void closeEvent(QCloseEvent *event);

private:
    SJ::MySocket* sock;
    Ui::TransferListDialog *ui;
    // ��ʱ����ÿ��һ����Ⱦһ���ļ��б�
    QTimer *my_timer;
};

#endif // TRANSFERLISTDIALOG_H
