#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QStackedWidget>
#include <vector>
#include "../../common/utils/carinfo.h"
#include "protocol/clientLobbyProtocol.h"
#include "../../common/thread/thread.h"
#include "guestwaiting.h"
#include "hostwaiting.h"
#include "joingame.h"
#include "mainmenu.h"
#include "newgame.h"

class ClientGameProtocol;
class Lobby;

// NOTE:
// This small auxiliary thread class is defined directly inside lobby.h because it
// is only used by the Lobby and not meant to be reused elsewhere.
//
// The Lobby must wait for a blocking start-signal from the server before closing
// the window and transitioning to the game. Since recv_start_signal() blocks
// indefinitely, executing it in the main Qt thread would freeze the UI and
// eventually crash the application.
//
// To avoid this, we run the blocking receive in a dedicated Thread subclass that
// performs no UI work. Once the start signal arrives, the thread notifies the Qt
// main thread using QMetaObject::invokeMethod(), ensuring the UI remains
// responsive and the window is closed safely.
//
// This localized helper thread allows the lobby flow to remain stable without
// introducing new files or exposing internal details outside this module.

class StartSignalThread : public Thread {
public:
    StartSignalThread(Lobby* l, ClientLobbyProtocol* p);
    void run() override;
private:
    Lobby* lobby;
    ClientLobbyProtocol* protocol;
};


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
    void setStartGame(bool value) { _startGame = value; }

private:
    Ui::Lobby* ui;
    QStackedWidget* stackedWidget;
    MainMenu* mainMenu;
    NewGame* newGame;
    JoinGame* joinGame;
    GuestWaiting* guestWaiting;
    HostWaiting* hostWaiting;
    ClientLobbyProtocol protocol; // Lobby has its own protocol
    std::vector<CarInfo> availableCars;
    uint16_t carId;
    bool _startGame = false;
    StartSignalThread startSignalThread;

    void handleJoinGameRequest(const QString& username, const QString& gameId, const CarInfo& car);
    void handleNewGameRequest(const QString& username, const CarInfo& car);
    void exitLobby();
    void closeEvent(QCloseEvent* event) override;
};
#endif  // LOBBY_H
