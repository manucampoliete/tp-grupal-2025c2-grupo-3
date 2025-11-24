#include "editor.h"
#include "./ui_editor.h"
#include "mapeditor.h"

#include <QScreen>
#include <QFontDatabase>

Editor::Editor(QWidget* parent): QMainWindow(parent), ui(new Ui::Editor) {
    ui->setupUi(this);

    this->setWindowTitle("Map Editor");
    this->resize(960, 540);

    ui->stackedWidget->setGeometry(0, 0, 1280, 720);

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

    connect(menu, &Menu::mapSelected, this, &Editor::launchMapEditor);

    ui->stackedWidget->setCurrentWidget(menu);
    this->setFixedSize(960, 540);
}

void Editor::launchMapEditor(int cityId) {
    MapEditor* mapEditor = new MapEditor(cityId, this);

    mapEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->stackedWidget->addWidget(mapEditor);
    ui->stackedWidget->setCurrentWidget(mapEditor);

    const int editorWidth = 1280;
    const int editorHeight = 720;

    this->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    this->resize(editorWidth, editorHeight);

    QScreen* screen = QGuiApplication::primaryScreen();
    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();
    int newX = (screenWidth - editorWidth) / 2;
    int newY = (screenHeight - editorHeight) / 2;
    this->move(newX, newY);

    this->setFixedSize(editorWidth, editorHeight);

    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(Qt::white));
    this->setPalette(palette);
    this->setAutoFillBackground(false);

    this->updateGeometry();
}

void Editor::exitEditor() {
    this->close();
}

Editor::~Editor() { delete ui; }
