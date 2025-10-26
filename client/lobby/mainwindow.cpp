#include "mainwindow.h"
#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "./ui_mainwindow.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QFontDatabase>
#include <ui_guestwaiting.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Need For Speed");
    this->setFixedSize(960, 540);

    //Código para centrar la ventana
    QScreen *screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int newX = (screenWidth - windowWidth) / 2;
    int newY = (screenHeight - windowHeight) / 2;

    this->move(newX, newY);


    MainMenu *main_menu = new MainMenu(this);
    NewGame *new_game = new NewGame(this);
    JoinGame *join_game = new JoinGame(this);
    guestWaiting *guest_waiting = new guestWaiting(this);
    HostWaiting *host_waiting = new HostWaiting(this);

    QStackedWidget *stackedWidget = new QStackedWidget(this);

    QPixmap bkgnd(":/media/menu_background_blurred.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    stackedWidget->addWidget(main_menu);  // índice 0
    stackedWidget->addWidget(new_game);     // índice 1
    stackedWidget->addWidget(join_game);    // indice 2
    stackedWidget->addWidget(guest_waiting);    // indice 3
    stackedWidget->addWidget(host_waiting);     // indice 4

    setCentralWidget(stackedWidget);

    connect(main_menu, &MainMenu::newGameClicked, this, [stackedWidget]() {
        stackedWidget->setCurrentIndex(1);
    });

    connect(main_menu, &MainMenu::joinGameClicked, this, [stackedWidget]() {
        stackedWidget->setCurrentIndex(2);
    });

    connect(main_menu, &MainMenu::exitClicked, qApp, &QApplication::quit);

    connect(join_game, &JoinGame::returnToMenuClicked, stackedWidget, [this, stackedWidget, main_menu](){
        stackedWidget->setCurrentIndex(0);
    });

    connect(new_game, &NewGame::returnToMenuClicked, stackedWidget, [this, stackedWidget, main_menu](){
        stackedWidget->setCurrentIndex(0);
    });

    connect(join_game, &JoinGame::joinGameRequested, stackedWidget, [this, stackedWidget, main_menu](){
        stackedWidget->setCurrentIndex(3);
    });

    connect(new_game, &NewGame::newGameRequested, stackedWidget, [this, stackedWidget, main_menu](){
        stackedWidget->setCurrentIndex(4);
    });

    stackedWidget->setCurrentWidget(main_menu);
}

void MainWindow::handleJoinGameRequest()
{
    stackedWidget->setCurrentWidget(guest_waiting);

    // Aquí podrías agregar lógica para iniciar la conexión de red
}

void MainWindow::handleNewGameRequest()
{
    stackedWidget->setCurrentWidget(host_waiting);

    // Aquí podrías agregar lógica para iniciar la conexión de red
}

MainWindow::~MainWindow()
{
    delete ui;
}
