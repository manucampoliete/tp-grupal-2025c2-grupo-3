#include "joingame.h"
#include "ui_joingame.h"

#include <QFontDatabase>

JoinGame::JoinGame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::JoinGame)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setAutoFillBackground(true);

    QFontDatabase::addApplicationFont(":/media/orbitron.ttf");

    carSelector = new CarSelector(this);
    ui->mainLayout->addWidget(carSelector);
}

void JoinGame::on_buttonReturn_clicked()
{
    emit returnToMenuClicked();
}

void JoinGame::on_buttonJoin_clicked()
{ /*
    // Asegúrate de que ui->carSelectorWidget apunte a la instancia del CarSelector
    CarSelector *selector = ui->carSelectorWidget; // O tu puntero miembro si lo creaste en C++

    // Obtener los datos del formulario (asume que tienes QLineEdit para ip y port)
    QString ip = ui->ipLineEdit->text();
    QString port = ui->portLineEdit->text();

    // Obtener el ID del auto actualmente visible
    QString selectedCarId = selector->getSelectedCarId();

    if (!selectedCarId.isEmpty() && !ip.isEmpty() && !port.isEmpty()) {
        qDebug() << "Lobby Listo. Auto ID:" << selectedCarId;

        // Emitir la señal de inicio de lobby a la MainWindow
        emit startLobby(selectedCarId, ip, port);

        // Opcional: Volver al MainMenu o cerrar JoinGame
    } else {
        QMessageBox::warning(this, "Error", "Faltan datos o no se seleccionó el auto.");
    } */

    emit joinGameRequested();
}

JoinGame::~JoinGame()
{
    delete ui;
}
