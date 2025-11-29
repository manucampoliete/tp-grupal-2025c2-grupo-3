#include "hostwaiting.h"
#include "ui_hostwaiting.h"
#include <QTimer>

HostWaiting::HostWaiting(QWidget* parent): QWidget(parent), ui(new Ui::HostWaiting) {
    ui->setupUi(this);

    static const QString baseText = "Waiting for players to join";
    static int dotCount = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        QString dots = QString(".").repeated(dotCount);
        ui->waitingLabel->setText(baseText + dots);
        dotCount = (dotCount + 1) % 8;
    });
    timer->start(500);
}

void HostWaiting::on_buttonStart_clicked() { emit startClicked(); }

void HostWaiting::setMatchID(QString matchId) {
    this->matchId = matchId;
    QString currentText = ui->labelGameID->text();
    ui->labelGameID->setText(currentText + "     " + matchId);
    ui->labelGameID->adjustSize();
}

HostWaiting::~HostWaiting() { delete ui; }
