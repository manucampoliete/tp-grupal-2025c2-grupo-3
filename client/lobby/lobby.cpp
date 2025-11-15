#include "lobby.h"

#include <QBrush>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QScreen>
#include <QStackedWidget>
#include <iostream>
#include <thread>

#include <ui_guestwaiting.h>

#include "./ui_lobby.h"

#include "guestwaiting.h"
#include "hostwaiting.h"
#include "joingame.h"
#include "mainmenu.h"
#include "newgame.h"

Lobby::Lobby(ClientProtocol& protocol, QWidget* parent):
        QMainWindow(parent), ui(new Ui::Lobby), protocol(protocol) {
    ui->setupUi(this);

    this->setWindowTitle("Need For Speed");
    this->setFixedSize(960, 540);
    this->setStyleSheet("QWidget { color: white; }");
    QScreen* screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int newX = (screenWidth - windowWidth) / 2;
    int newY = (screenHeight - windowHeight) / 2;

    this->move(newX, newY);

    available_cars = protocol.recv_initial_info();

    main_menu = new MainMenu(this);
    new_game = new NewGame(this, available_cars);
    join_game = new JoinGame(this, available_cars);
    guest_waiting = new guestWaiting(this);
    host_waiting = new HostWaiting(this);

    stackedWidget = new QStackedWidget(this);

    QPixmap bkgnd(":/media/menu_background_blurred.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    stackedWidget->addWidget(main_menu);      // índice 0
    stackedWidget->addWidget(new_game);       // índice 1
    stackedWidget->addWidget(join_game);      // indice 2
    stackedWidget->addWidget(guest_waiting);  // indice 3
    stackedWidget->addWidget(host_waiting);   // indice 4

    setCentralWidget(stackedWidget);

    connect(main_menu, &MainMenu::newGameClicked, this,
            [this]() { stackedWidget->setCurrentIndex(1); });

    connect(main_menu, &MainMenu::joinGameClicked, this,
            [this]() { stackedWidget->setCurrentIndex(2); });

    connect(main_menu, &MainMenu::exitClicked, this, &Lobby::exitLobby);

    connect(join_game, &JoinGame::returnToMenuClicked, this,
            [this]() { stackedWidget->setCurrentIndex(0); });

    connect(join_game, &JoinGame::joinGameRequested, this, &Lobby::handleJoinGameRequest);

    connect(new_game, &NewGame::returnToMenuClicked, this,
            [this]() { stackedWidget->setCurrentIndex(0); });

    connect(new_game, &NewGame::newGameRequested, this, &Lobby::handleNewGameRequest);

    connect(host_waiting, &HostWaiting::startClicked, this, &Lobby::startGame);

    stackedWidget->setCurrentWidget(main_menu);
}

void Lobby::handleJoinGameRequest(const QString& username, const QString& gameId,
                                  const CarInfo& car) {
    bool joined = protocol.send_join(static_cast<uint16_t>(gameId.toInt()), username.toStdString(), static_cast<uint8_t>(car.id));

    if (!joined) {
        join_game->setJoinError();
        return;
    }

    car_id = static_cast<uint16_t>(car.id);
    
    guest_waiting->setMatchID(gameId);
    stackedWidget->setCurrentWidget(guest_waiting);
    std::thread([this]() {
        protocol.recv_start_signal();
        QMetaObject::invokeMethod(this, [this]() { this->close(); });
    }).detach();
}

void Lobby::handleNewGameRequest(const QString& username, const CarInfo& car)
{
    uint16_t match_id = protocol.send_create(username.toStdString(), static_cast<uint8_t>(car.id));
    car_id = static_cast<uint16_t>(car.id);

    host_waiting->setMatchID(QString::number(match_id));
    stackedWidget->setCurrentWidget(host_waiting);
}

void Lobby::startGame() {
    protocol.send_start();
    
    std::thread([this]() {
        protocol.recv_start_signal();
        start_game = true;

        QMetaObject::invokeMethod(this, [this]() {
            this->close();
        });
    }).detach();
}

void Lobby::exitLobby() {
    start_game = false;
    this->close();
}

void Lobby::closeEvent(QCloseEvent* event) {
    if (!start_game) {
        start_game = false;
    }
    QMainWindow::closeEvent(event);
}

Lobby::~Lobby() { delete ui; }
