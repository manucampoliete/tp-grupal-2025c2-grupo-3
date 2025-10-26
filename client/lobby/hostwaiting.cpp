#include "hostwaiting.h"
#include "ui_hostwaiting.h"

HostWaiting::HostWaiting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HostWaiting)
{
    ui->setupUi(this);
}

HostWaiting::~HostWaiting()
{
    delete ui;
}
