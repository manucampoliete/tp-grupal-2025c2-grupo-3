#ifndef CARCARD_H
#define CARCARD_H

#include "../../common/utils/carinfo.h"

#include <QWidget>
#include <QString>
#include <QLabel>

class CarCard : public QWidget
{
    Q_OBJECT

public:
    explicit CarCard(const CarInfo &car, QWidget *parent = nullptr);
    ~CarCard();


private:
    QLabel *imageLabel;
    QLabel *nameLabel;
    QLabel *healthLabel;
    QLabel *speedLabel;
};

#endif // CARCARD_H

