#ifndef CARCARD_H
#define CARCARD_H

#include <QLabel>
#include <QString>
#include <QWidget>

#include "../../common/utils/carinfo.h"

class CarCard: public QWidget {
    Q_OBJECT

public:
    explicit CarCard(const CarInfo& car, QWidget* parent = nullptr);
    ~CarCard();


private:
    QLabel* imageLabel;
    QLabel* nameLabel;
    QLabel* healthLabel;
    QLabel* speedLabel;
};

#endif  // CARCARD_H
