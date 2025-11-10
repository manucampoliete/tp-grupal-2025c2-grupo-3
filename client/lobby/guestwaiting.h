#ifndef GUESTWAITING_H
#define GUESTWAITING_H

#include <QWidget>

namespace Ui {
class guestWaiting;
}

class guestWaiting : public QWidget
{
    Q_OBJECT

public:
    explicit guestWaiting(QWidget *parent = nullptr);
    ~guestWaiting();

private:
    Ui::guestWaiting *ui;
};

#endif // GUESTWAITING_H
