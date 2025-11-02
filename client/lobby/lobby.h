#ifndef LOBBY_H
#define LOBBY_H

#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "carinfo.h"

#include <QMainWindow>
#include <QStackedWidget>
//include "client_protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Lobby;
}
QT_END_NAMESPACE

class Lobby : public QMainWindow
{
    Q_OBJECT

public:
    Lobby(QWidget *parent = nullptr);
    ~Lobby();
    void initiate_connection(const std::string& ip, const std::string& port);
    void startGame();

    //ClientProtocol&& get_protocol();

private:
    Ui::Lobby *ui;
    QStackedWidget *stackedWidget;
    MainMenu* main_menu;
    NewGame* new_game;
    JoinGame* join_game;
    guestWaiting* guest_waiting;
    HostWaiting* host_waiting;
    //std::optional<ClientProtocol> protocol;
    std::vector<CarInfo> available_cars;

    void handleJoinGameRequest(const QString &username, const QString &gameId, const CarInfo &car);
    void handleNewGameRequest(const QString &username, const std::string& car);
};
#endif // LOBBY_H
