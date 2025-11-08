#ifndef LOBBY_H
#define LOBBY_H

#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "carinfo.h"
#include "../../common/protocol/client_protocol.h"

#include <QMainWindow>
#include <QStackedWidget>

class ClientProtocol;

QT_BEGIN_NAMESPACE
namespace Ui {
class Lobby;
}
QT_END_NAMESPACE

class Lobby : public QMainWindow
{
    Q_OBJECT

public:
    Lobby(ClientProtocol& protocol, QWidget *parent = nullptr);
    ~Lobby();
    void initiate_connection();
    void startGame();


private:
    Ui::Lobby *ui;
    QStackedWidget *stackedWidget;
    MainMenu* main_menu;
    NewGame* new_game;
    JoinGame* join_game;
    guestWaiting* guest_waiting;
    HostWaiting* host_waiting;
    ClientProtocol& protocol;
    std::vector<CarInfo> available_cars;

    void handleJoinGameRequest(const QString &username, const QString &gameId, const CarInfo &car);
    void handleNewGameRequest(const QString &username, const CarInfo &car);
    void wait_start();
};
#endif // LOBBY_H
