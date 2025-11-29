#include "mapoption.h"
#include <QMouseEvent>
#include <QPixmap>

MapOption::MapOption(const QString& title, const QString& imagePath, const int id, QWidget* parent): QFrame(parent), selected(false), cityId(id) {
    setObjectName("mapOptionRoot");
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setFixedSize(150, 200);

    imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(QPixmap(imagePath).scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    titleLabel = new QLabel(title);
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    titleLabel->setMinimumWidth(0);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    layout->addWidget(titleLabel);

    setSelected(false);
}

void MapOption::setSelected(bool sel) {
    selected = sel;

    if (sel) {
        setStyleSheet(
                "QWidget#mapOptionRoot {"
                "   border: 3px solid #FFD700;"
                "   border-radius: 8px;"
                "}"
                );
    } else {
        setStyleSheet(
                "QWidget#mapOptionRoot {"
                "   border: 2px solid transparent;"
                "   border-radius: 8px;"
                "}"
                );
    }
}

void MapOption::mousePressEvent(QMouseEvent* event) {
    emit clicked(this);
    QFrame::mousePressEvent(event);
}
