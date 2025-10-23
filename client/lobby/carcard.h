#ifndef CARCARD_H
#define CARCARD_H

#include <QWidget>
#include <QString>
#include <QLabel>

class CarCard : public QWidget
{
    Q_OBJECT

public:
    explicit CarCard(const QString &carId, QWidget *parent = nullptr);
    ~CarCard();

    QString getCarId() const { return m_carId; }

private:
    QString m_carId;
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLabel *healthLabel;
    QLabel *speedLabel;
};

#endif // CARCARD_H

