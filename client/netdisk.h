#ifndef NETDISK_H
#define NETDISK_H

#include <QMainWindow>
#include <QStringList>
#include <QCloseEvent>

#include "mysocket.h"
#include "transferlistdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NetDisk; }
QT_END_NAMESPACE

class NetDisk : public QMainWindow
{
    Q_OBJECT

public:
    NetDisk(QWidget *parent = nullptr);
    ~NetDisk();

protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void on_refresh_btn_clicked();

    void on_return_btn_clicked();

    void on_upload_file_clicked();

    void on_upload_dir_clicked();

    void on_new_dir_clicked();

    void on_file_list_cellClicked(int row, int column);

    void on_file_list_cellDoubleClicked(int row, int column);

    void on_paste_btn_clicked();

    void on_transfer_list_menu_triggered();

private:
    SJ::MySocket* sock;
    Ui::NetDisk *ui;
    TransferListDialog *transfer_dialog;
    QStringList file_list, dir_list;
    QString download_file, upload_file;
    QString user, path;

    void showMsg(QString msg);
    void renderFileList(QStringList file_list, QStringList dir_list);
    void reloadFile();
};
#endif // NETDISK_H
