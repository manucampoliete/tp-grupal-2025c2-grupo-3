#include "guestwaiting.h"
#include "ui_guestwaiting.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>

GuestWaiting::GuestWaiting(QWidget* parent): QWidget(parent), ui(new Ui::GuestWaiting) {
    ui->setupUi(this);

    static const QString baseText = "Waiting for game to start";
    static int dotCount = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        QString dots = QString(".").repeated(dotCount);
        ui->waitingLabel->setText(baseText + dots);
        dotCount = (dotCount + 1) % 8;
    });

    timer->start(500);
}

void GuestWaiting::setMatchID(QString matchId) {
    this->matchId = matchId;
    QString currentText = ui->idLabel->text();
    ui->idLabel->setText(currentText + "     " + matchId);
    ui->idLabel->adjustSize();
}

GuestWaiting::~GuestWaiting() { delete ui; }
