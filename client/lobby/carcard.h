#ifndef CARCARD_H
#define CARCARD_H

#include <QWidget>
#include <QString>
#include <QLabel>

namespace Ui {
class CarCard;
}

class CarCard : public QWidget
{
    Q_OBJECT

public:
    explicit CarCard(const QString &carId, QWidget *parent = nullptr);
    ~CarCard();

    QString getCarId() const { return m_carId; }

private:
    Ui::CarCard *ui;
    QString m_carId;
};

#endif // CARCARD_H

