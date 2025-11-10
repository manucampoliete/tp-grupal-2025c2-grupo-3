#ifndef CARSELECTOR_H
#define CARSELECTOR_H

#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>
#include <vector>

#include "../../common/utils/carinfo.h"

class CarCard;

class CarSelector: public QWidget {
    Q_OBJECT

public:
    explicit CarSelector(QWidget* parent = nullptr, const std::vector<CarInfo>& cars = {});
    ~CarSelector();

    CarInfo getSelectedCar() const;
    void setupCars(const std::vector<CarInfo>& cars);

private slots:
    void on_prevButton_clicked();
    void on_nextButton_clicked();

private:
    QStackedWidget* carStack;
    QPushButton* prevButton;
    QPushButton* nextButton;
    std::vector<CarInfo> car_list;
};

#endif  // CARSELECTOR_H
