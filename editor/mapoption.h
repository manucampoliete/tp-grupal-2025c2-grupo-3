#ifndef MAPOPTION_H
#define MAPOPTION_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class MapOption : public QFrame {
    Q_OBJECT

public:
    explicit MapOption(const QString& title, const QString& imagePath, const int cityId, QWidget* parent = nullptr);

    void setSelected(bool sel);
    bool isSelected() const { return selected; }
    int getCityId() const { return cityId; }

signals:
    void clicked(MapOption* self);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool selected;
    QLabel* imageLabel;
    QLabel* titleLabel;
    int cityId;
};

#endif
