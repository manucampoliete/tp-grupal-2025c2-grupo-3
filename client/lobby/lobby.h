#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QStackedWidget>
#include <vector>

#include "../../common/utils/carinfo.h"
#include "protocol/clientLobbyProtocol.h"

#include "guestwaiting.h"
#include "hostwaiting.h"
#include "joingame.h"
#include "mainmenu.h"
#include "newgame.h"

class ClientGameProtocol;

QT_BEGIN_NAMESPACE
namespace Ui {
class Lobby;
}
QT_END_NAMESPACE

class Lobby: public QMainWindow {
    Q_OBJECT

public:
    explicit Lobby(Socket& skt, QWidget* parent = nullptr);
    ~Lobby();
    void startGame();
    uint16_t getCarID() const { return carId; }
    bool shouldStartGame() const { return _startGame; }

private:
    Ui::Lobby* ui;
    QStackedWidget* stackedWidget;
    MainMenu* mainMenu;
    NewGame* newGame;
    JoinGame* joinGame;
    GuestWaiting* guestWaiting;
    HostWaiting* hostWaiting;
    ClientLobbyProtocol protocol;  // Lobby has its own protocol
    std::vector<CarInfo> availableCars;
    uint16_t carId;
    bool _startGame = false;

    void handleJoinGameRequest(const QString& username, const QString& gameId, const CarInfo& car);
    void handleNewGameRequest(const QString& username, const CarInfo& car);
    void exitLobby();
    void closeEvent(QCloseEvent* event) override;
};
#endif  // LOBBY_H
