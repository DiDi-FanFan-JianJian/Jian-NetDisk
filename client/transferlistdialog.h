#ifndef TRANSFERLISTDIALOG_H
#define TRANSFERLISTDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class TransferListDialog;
}

class TransferListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferListDialog(QWidget *parent = nullptr);
    ~TransferListDialog();

    void on_upload_file_list_cellClicked(int row, int column);

    void on_download_file_list_cellClicked(int row, int column);

    void renderFileList();    // ��Ⱦ���еĴ����е��ļ�

private slots:
    void my_timer_timeout();

private:
    Ui::TransferListDialog *ui;
    // ��ʱ����ÿ��һ����Ⱦһ���ļ��б�
    QTimer *my_timer;
};

#endif // TRANSFERLISTDIALOG_H
