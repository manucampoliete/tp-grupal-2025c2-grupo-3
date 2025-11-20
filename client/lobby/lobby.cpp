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

    availableCars = protocol.recvInitialInfo();

    mainMenu = new MainMenu(this);
    newGame = new NewGame(this, availableCars);
    joinGame = new JoinGame(this, availableCars);
    guestWaiting = new GuestWaiting(this);
    hostWaiting = new HostWaiting(this);

    stackedWidget = new QStackedWidget(this);

    QPixmap bkgnd(":/media/menu_background_blurred.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    stackedWidget->addWidget(mainMenu);      // Index -> 0
    stackedWidget->addWidget(newGame);       // Index -> 1
    stackedWidget->addWidget(joinGame);      // Index -> 2
    stackedWidget->addWidget(guestWaiting);  // Index -> 3
    stackedWidget->addWidget(hostWaiting);   // Index -> 4
    setCentralWidget(stackedWidget);

    connect(mainMenu, &MainMenu::newGameClicked, this,
            [this]() { stackedWidget->setCurrentIndex(1); });

    connect(mainMenu, &MainMenu::joinGameClicked, this,
            [this]() { stackedWidget->setCurrentIndex(2); });

    connect(mainMenu, &MainMenu::exitClicked, this, &Lobby::exitLobby);

    connect(joinGame, &JoinGame::returnToMenuClicked, this,
            [this]() { stackedWidget->setCurrentIndex(0); });

    connect(joinGame, &JoinGame::joinGameRequested, this, &Lobby::handleJoinGameRequest);

    connect(newGame, &NewGame::returnToMenuClicked, this,
            [this]() { stackedWidget->setCurrentIndex(0); });

    connect(newGame, &NewGame::newGameRequested, this, &Lobby::handleNewGameRequest);

    connect(hostWaiting, &HostWaiting::startClicked, this, &Lobby::startGame);

    stackedWidget->setCurrentWidget(mainMenu);
}

void Lobby::handleJoinGameRequest(const QString& username, const QString& gameId,
                                  const CarInfo& car) {
    bool joined = protocol.sendJoin(static_cast<uint16_t>(gameId.toInt()), username.toStdString(), static_cast<uint8_t>(car.id));

    if (!joined) {
        joinGame->setJoinError();
        return;
    }

    carId = static_cast<uint16_t>(car.id);
    
    guestWaiting->setMatchID(gameId);
    stackedWidget->setCurrentWidget(guestWaiting);
    std::thread([this]() {
        protocol.recvStartSignal();
        _startGame = true;
        QMetaObject::invokeMethod(this, [this]() { this->close(); });
    }).detach();
}

void Lobby::handleNewGameRequest(const QString& username, const CarInfo& car)
{
    uint16_t matchId = protocol.sendCreate(username.toStdString(), static_cast<uint8_t>(car.id));
    carId = static_cast<uint16_t>(car.id);

    hostWaiting->setMatchID(QString::number(matchId));
    stackedWidget->setCurrentWidget(hostWaiting);
}

void Lobby::startGame() {
    protocol.sendStart();
    
    std::thread([this]() {
        protocol.recvStartSignal();
        _startGame = true;

        QMetaObject::invokeMethod(this, [this]() {
            this->close();
        });
    }).detach();
}

void Lobby::exitLobby() {
    _startGame = false;
    this->close();
}

void Lobby::closeEvent(QCloseEvent* event) {
    if (!_startGame) {
        _startGame = false;
    }
    QMainWindow::closeEvent(event);
}

Lobby::~Lobby() { delete ui; }
