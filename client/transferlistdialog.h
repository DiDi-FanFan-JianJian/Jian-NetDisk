#ifndef TRANSFERLISTDIALOG_H
#define TRANSFERLISTDIALOG_H

#include <QDialog>

namespace Ui {
class TransferListDialog;
}

class TransferListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferListDialog(QWidget *parent = nullptr);
    ~TransferListDialog();

private:
    Ui::TransferListDialog *ui;
};

#endif // TRANSFERLISTDIALOG_H
