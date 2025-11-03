#include "lobby.h"
#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "./ui_lobby.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QFontDatabase>
#include <ui_guestwaiting.h>
#include <iostream>

Lobby::Lobby(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Lobby)
{
    ui->setupUi(this);

    this->setWindowTitle("Need For Speed");
    this->setFixedSize(960, 540);

    QScreen *screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int newX = (screenWidth - windowWidth) / 2;
    int newY = (screenHeight - windowHeight) / 2;

    this->move(newX, newY);

    main_menu = new MainMenu(this);
    new_game = new NewGame(this);
    join_game = new JoinGame(this);
    guest_waiting = new guestWaiting(this);
    host_waiting = new HostWaiting(this);

    stackedWidget = new QStackedWidget(this);

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

    connect(main_menu, &MainMenu::newGameClicked, this, [this]() {
        stackedWidget->setCurrentIndex(1); 
    });

    connect(main_menu, &MainMenu::joinGameClicked, this, [this]() {
        stackedWidget->setCurrentIndex(2); 
    });

    connect(main_menu, &MainMenu::exitClicked, qApp, &QApplication::quit);

    connect(join_game, &JoinGame::returnToMenuClicked, this, [this]() {
        stackedWidget->setCurrentIndex(0); 
    });

    connect(join_game, &JoinGame::joinGameRequested, this, &Lobby::handleJoinGameRequest);

    connect(new_game, &NewGame::returnToMenuClicked, this, [this]() {
        stackedWidget->setCurrentIndex(0); 
    });

    connect(new_game, &NewGame::newGameRequested, this, &Lobby::handleNewGameRequest);

    connect(host_waiting, &HostWaiting::startClicked, this, &Lobby::startGame);

    stackedWidget->setCurrentWidget(main_menu);
}

void Lobby::initiate_connection(const std::string& ip, const std::string& port){
    try {
        std::cout << "Iniciando conexión a " << ip << ":" << port << "...\n";
        //Socket socket(ip.c_str(), port.c_str());
        //protocol.emplace(std::move(socket));

        //available_cars = protocol.receive_car_info();
    } catch (const std::exception& e) {
        std::cerr << "Error al conectar con el servidor: " << e.what() << std::endl;
        throw;
    }
}

void Lobby::handleJoinGameRequest(const QString &username, const QString &gameId, const CarInfo &car)
{
    //bool joined = protocol->sendJoinGameRequest(username.toStdString(), gameId.toStdString(), car.id);
    std::cout << "El jugador " << username.toStdString()
              << " quiere unirse al juego " << gameId.toStdString()
              << " con el auto " << car.name << std::endl << std::flush;

    stackedWidget->setCurrentWidget(guest_waiting);
    //recv_start_signal();
}

void Lobby::handleNewGameRequest(const QString &username, const CarInfo &car) //car pasarlo a id y a uint
{   
    //uint16_t match_id = protocol->send_create(username.toStdString(), car.id);
    std::cout << "Nuevo juego solicitado por " << username.toStdString()
              << " con el auto " << car.name << std::endl << std::flush;

    stackedWidget->setCurrentWidget(host_waiting);
}

void Lobby::startGame()
{
    //bool started = protocol->send_start();
    
    std::cout << "El host ha comenzado la partida!" << std::endl;
    
    this->close();
    
    //runSDLGame(get_protocol()); 
}

/* ClientProtocol&& Lobby::get_protocol() {
    return std::move(protocol.value());
} */

Lobby::~Lobby()
{
    delete ui;
}
