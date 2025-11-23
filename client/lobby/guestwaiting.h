#ifndef GUESTWAITING_H
#define GUESTWAITING_H

#include <QWidget>

namespace Ui {
class GuestWaiting;
}

class GuestWaiting: public QWidget {
    Q_OBJECT

public:
    explicit GuestWaiting(QWidget* parent = nullptr);
    ~GuestWaiting();

    void setMatchID(QString matchId);

private:
    Ui::GuestWaiting* ui;
    QString matchId;
};

#endif  // GUESTWAITING_H
