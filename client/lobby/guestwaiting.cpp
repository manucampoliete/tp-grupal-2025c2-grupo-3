#include "guestwaiting.h"
#include "ui_guestwaiting.h"

#include <QMovie>
#include <QLabel>

guestWaiting::guestWaiting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::guestWaiting)
{
    ui->setupUi(this);

    QMovie *loadingMovie = new QMovie(":/media/loading.gif", QByteArray(), this);
    QLabel *loadingLabel = new QLabel(this);
    QHBoxLayout *hLayout = new QHBoxLayout();

    hLayout->addStretch(3);

    loadingLabel->setFixedSize(120, 120);

    loadingLabel->setMovie(loadingMovie);
    hLayout->addWidget(loadingLabel);
    loadingMovie->start();

    hLayout->addStretch(1);

    hLayout->setContentsMargins(10, 0, 70, 0);

    ui->mainLayout->addLayout(hLayout);
}
guestWaiting::~guestWaiting()
{
    delete ui;
}
