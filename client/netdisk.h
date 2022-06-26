#ifndef NETDISK_H
#define NETDISK_H

#include <QMainWindow>
#include <QAction>
#include <QTreeWidgetItem>

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

    void on_rename_btn_clicked();

    void on_copy_btn_clicked();

    void on_delete_btn_clicked();

    void on_move_btn_clicked();

private:
    Ui::NetDisk *ui;
    QStringList file_list;
    QString download_file, upload_file;

    void showMsg(QString msg);
};
#endif // NETDISK_H
