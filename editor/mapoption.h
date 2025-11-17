#ifndef MAPOPTION_H
#define MAPOPTION_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class MapOption : public QFrame {
    Q_OBJECT

public:
    explicit MapOption(const QString& title,
                             const QString& imagePath,
                             QWidget* parent = nullptr);

    void setSelected(bool sel);
    bool isSelected() const { return selected; }

signals:
    void clicked(MapOption* self);
    void doubleClicked(MapOption* self);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    bool selected;
    QLabel* imageLabel;
    QLabel* titleLabel;
};

#endif
