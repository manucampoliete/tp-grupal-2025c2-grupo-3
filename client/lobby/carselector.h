#ifndef CARSELECTOR_H
#define CARSELECTOR_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>

class CarCard;

namespace Ui {
class CarSelector;
}

class CarSelector : public QWidget
{
    Q_OBJECT

public:
    explicit CarSelector(QWidget *parent = nullptr);
    ~CarSelector();

    QString getSelectedCarId() const;

private slots:
    void on_prevButton_clicked();
    void on_nextButton_clicked();

private:
    Ui::CarSelector *ui;
    QStackedWidget *carStack;
    QPushButton *prevButton;
    QPushButton *nextButton;
    void setupCars();
};

#endif // CARSELECTOR_H
