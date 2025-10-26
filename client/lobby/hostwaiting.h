#ifndef HOSTWAITING_H
#define HOSTWAITING_H

#include <QWidget>

namespace Ui {
class HostWaiting;
}

class HostWaiting : public QWidget
{
    Q_OBJECT

public:
    explicit HostWaiting(QWidget *parent = nullptr);
    ~HostWaiting();

private:
    Ui::HostWaiting *ui;
};

#endif // HOSTWAITING_H
