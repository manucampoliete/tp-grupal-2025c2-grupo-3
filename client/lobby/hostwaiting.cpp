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

void HostWaiting::setMatchID(QString match_id)
{
    this->match_id = match_id;
    QString currentText = ui->labelGameID->text();
    ui->labelGameID->setText(currentText + "   " + match_id);
}

HostWaiting::~HostWaiting()
{
    delete ui;
}
