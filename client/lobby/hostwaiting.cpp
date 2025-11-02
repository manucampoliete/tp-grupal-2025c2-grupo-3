#include "hostwaiting.h"
#include "ui_hostwaiting.h"

HostWaiting::HostWaiting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HostWaiting)
{
    ui->setupUi(this);
}

void HostWaiting::on_buttonStart_clicked()
{
    emit startClicked();
}

HostWaiting::~HostWaiting()
{
    delete ui;
}
