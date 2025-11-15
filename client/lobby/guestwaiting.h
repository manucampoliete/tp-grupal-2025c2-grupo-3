#ifndef GUESTWAITING_H
#define GUESTWAITING_H

#include <QWidget>

namespace Ui {
class guestWaiting;
}

class guestWaiting: public QWidget {
    Q_OBJECT

public:
    explicit guestWaiting(QWidget* parent = nullptr);
    ~guestWaiting();

    void setMatchID(QString match_id);

private:
    Ui::guestWaiting* ui;
    QString match_id;
};

#endif  // GUESTWAITING_H
