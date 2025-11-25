#include "editor.h"
#include "./ui_editor.h"
#include "mapeditor.h"

#include <QScreen>
#include <QFontDatabase>
#include <QDir>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <yaml-cpp/yaml.h>

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

    connect(menu, &Menu::mapOpenRequested, this, &Editor::openMapFileDialog);

    connect(menu, &Menu::mapSelected, this, &Editor::launchMapEditor);

    ui->stackedWidget->setCurrentWidget(menu);
    this->setFixedSize(960, 540);
}

void Editor::launchMapEditor(int cityId) {
    const QString& finalPath = filePathToLoad;
    MapEditor* mapEditor = new MapEditor(cityId, finalPath, this);

    if (!filePathToLoad.isEmpty()) {
        filePathToLoad = QString();
    }

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

int Editor::loadMapIdFromYaml(const QString& filePath) {
    try {
        YAML::Node root = YAML::LoadFile(filePath.toStdString());
        return root["map_name"].as<int>();
    } catch (const YAML::Exception& e) {
        qWarning() << "YAML Read Error (ID):" << e.what();
        return -1;
    }
}

void Editor::openMapFileDialog() {
    QString filter = "Archivos de Mapa YAML (*.yaml *.yml)";
    QString startPath = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, "Abrir Mapa de Carrera (YAML)", startPath, filter);

    if (!fileName.isEmpty()) {
        filePathToLoad = fileName;
        launchMapEditor(0);
    } else {
        std::cout << "Carga de mapa cancelada." << std::endl;
    }
}

void Editor::exitEditor() {
    this->close();
}

Editor::~Editor() { delete ui; }
