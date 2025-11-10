#ifndef LOBBY_H
#define LOBBY_H

#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "../../common/utils/carinfo.h"
#include "../protocol/client_protocol.h"

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
    void startGame();
    uint16_t getCarID() const { return car_id; }

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
    uint16_t car_id;

    void handleJoinGameRequest(const QString &username, const QString &gameId, const CarInfo &car);
    void handleNewGameRequest(const QString &username, const CarInfo &car);
};
#endif // LOBBY_H
