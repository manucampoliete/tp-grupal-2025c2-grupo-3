#include "editor.h"
#include "./ui_editor.h"

#include <QScreen>
#include <QFontDatabase>

Editor::Editor(QWidget* parent): QMainWindow(parent), ui(new Ui::Editor) {
    ui->setupUi(this);

    this->setWindowTitle("Map Editor");
    this->setFixedSize(960, 540);
    QScreen* screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int newX = (screenWidth - windowWidth) / 2;
    int newY = (screenHeight - windowHeight) / 2;

    this->move(newX, newY);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");

    QPixmap bkgnd(":/media/menu_background_blurred.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    menu = new Menu(this);

    ui->stackedWidget->addWidget(menu);

    connect(menu, &Menu::exitClicked, this, &Editor::exitEditor);

    ui->stackedWidget->setCurrentWidget(menu);
}

void Editor::exitEditor() {
    this->close();
}

Editor::~Editor() { delete ui; }
