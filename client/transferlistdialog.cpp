#include "transferlistdialog.h"
#include "ui_transferlistdialog.h"

TransferListDialog::TransferListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferListDialog)
{
    ui->setupUi(this);
}

TransferListDialog::~TransferListDialog()
{
    delete ui;
}
