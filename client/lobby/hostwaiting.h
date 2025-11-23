#ifndef HOSTWAITING_H
#define HOSTWAITING_H

#include <QWidget>

namespace Ui {
class HostWaiting;
}

class HostWaiting: public QWidget {
    Q_OBJECT

public:
    explicit HostWaiting(QWidget* parent = nullptr);
    ~HostWaiting();

    void setMatchID(QString matchId);

signals:
    void startClicked();

private slots:
    void on_buttonStart_clicked();

private:
    Ui::HostWaiting* ui;
    QString matchId;
};

#endif  // HOSTWAITING_H
