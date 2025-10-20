#include "mainwindow.h"
#include "mainmenu.h"
#include "newgame.h"
#include "joingame.h"
#include "./ui_mainwindow.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Need For Speed");
    this->setFixedSize(960, 540);

    //Código para centrar la ventana
    QScreen *screen = QGuiApplication::primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int newX = (screenWidth - windowWidth) / 2;
    int newY = (screenHeight - windowHeight) / 2;

    this->move(newX, newY);


    MainMenu *main_menu = new MainMenu(this);
    NewGame *new_game = new NewGame(this);
    JoinGame *join_game = new JoinGame(this);


    QStackedWidget *stackedWidget = new QStackedWidget(this);

    QPixmap bkgnd(":/media/menu_background_blurred.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    this->setAutoFillBackground(true);


    stackedWidget->addWidget(main_menu);  // índice 0
    stackedWidget->addWidget(new_game);     // índice 1
    stackedWidget->addWidget(join_game);    // indice 2

    setCentralWidget(stackedWidget);

    connect(main_menu, &MainMenu::newGameClicked, this, [stackedWidget]() {
        stackedWidget->setCurrentIndex(1);
    });

    connect(main_menu, &MainMenu::joinGameClicked, this, [stackedWidget]() {
        stackedWidget->setCurrentIndex(2);
    });

    connect(main_menu, &MainMenu::exitClicked, qApp, &QApplication::quit);

    stackedWidget->setCurrentWidget(main_menu);
}

MainWindow::~MainWindow()
{
    delete ui;
}
