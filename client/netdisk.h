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
    QAction * test;   //自定义的QAction

private slots:
    void testSlot();   //与QAction想关联的槽函数
    void itemPressedSlot(QTreeWidgetItem *, int);   //用来判断是否是鼠标右键按下的槽函数

    void on_refresh_btn_clicked();

    void on_return_btn_clicked();

    void on_upload_file_clicked();

    void on_upload_dir_clicked();

private:
    Ui::NetDisk *ui;

    void showMsg(QString msg);
};
#endif // NETDISK_H
