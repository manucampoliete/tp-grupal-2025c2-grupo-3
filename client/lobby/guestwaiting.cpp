#include "guestwaiting.h"
#include "ui_guestwaiting.h"

#include <QLabel>
#include <QMovie>
#include <QTimer>

guestWaiting::guestWaiting(QWidget* parent): QWidget(parent), ui(new Ui::guestWaiting) {
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

void guestWaiting::setMatchID(QString match_id) {
    this->match_id = match_id;
    QString currentText = ui->idLabel->text();
    ui->idLabel->setText(currentText + "     " + match_id);
    ui->idLabel->adjustSize();
}

guestWaiting::~guestWaiting() { delete ui; }
