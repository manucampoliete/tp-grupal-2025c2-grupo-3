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

    void setMatchID(QString match_id);

signals:
    void startClicked();

private slots: 
    void on_buttonStart_clicked();

private:
    Ui::HostWaiting *ui;
    QString match_id;
};

#endif // HOSTWAITING_H
