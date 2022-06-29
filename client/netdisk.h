#ifndef NETDISK_H
#define NETDISK_H

#include <QMainWindow>
#include <QStringList>

#include "mysocket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NetDisk; }
QT_END_NAMESPACE

class NetDisk : public QMainWindow
{
    Q_OBJECT

public:
    NetDisk(QWidget *parent = nullptr);
    ~NetDisk();

private slots:
    void on_refresh_btn_clicked();

    void on_return_btn_clicked();

    void on_upload_file_clicked();

    void on_upload_dir_clicked();

    void on_new_dir_clicked();

    void on_file_list_cellClicked(int row, int column);

    void on_file_list_cellDoubleClicked(int row, int column);

    void on_copy_dir_btn_clicked();

    void on_move_file_btn_clicked();

    void on_copy_file_btn_clicked();

    void on_move_dir_btn_clicked();

private:
    SJ::MySocket* sock;
    Ui::NetDisk *ui;
    QStringList file_list, dir_list;
    QString download_file, upload_file;
    QString user, path;

    void showMsg(QString msg);
    void renderFileList(QStringList file_list, QStringList dir_list);
    void reloadFile();
};
#endif // NETDISK_H
