#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QStackedWidget>
#include <vector>

#include "../../common/utils/carinfo.h"
#include "../protocol/client_protocol.h"

#include "guestwaiting.h"
#include "hostwaiting.h"
#include "joingame.h"
#include "mainmenu.h"
#include "newgame.h"

class ClientProtocol;

QT_BEGIN_NAMESPACE
namespace Ui {
class Lobby;
}
QT_END_NAMESPACE

class Lobby: public QMainWindow {
    Q_OBJECT

public:
    explicit Lobby(ClientProtocol& protocol, QWidget* parent = nullptr);
    ~Lobby();
    void startGame();
    uint16_t getCarID() const { return car_id; }
    bool shouldStartGame() const { return start_game; }

private:
    Ui::Lobby* ui;
    QStackedWidget* stackedWidget;
    MainMenu* main_menu;
    NewGame* new_game;
    JoinGame* join_game;
    guestWaiting* guest_waiting;
    HostWaiting* host_waiting;
    ClientProtocol& protocol;
    std::vector<CarInfo> available_cars;
    uint16_t car_id;
    bool start_game = false;

    void handleJoinGameRequest(const QString& username, const QString& gameId, const CarInfo& car);
    void handleNewGameRequest(const QString& username, const CarInfo& car);
    void exitLobby();
    void closeEvent(QCloseEvent* event) override;
};
#endif  // LOBBY_H
