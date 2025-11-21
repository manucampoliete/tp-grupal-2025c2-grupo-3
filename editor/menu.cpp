#include "menu.h"
#include <iostream>
#include "mapeditor.h"
#include "ui_menu.h"

Menu::Menu(QWidget* parent): QWidget(parent), ui(new Ui::Menu) {
    ui->setupUi(this);

    ui->menuContainer->setFixedSize(850, 450);

    options[0] = new MapOption("Liberty City", ":/media/liberty.jpg", this);
    options[1] = new MapOption("San Andreas", ":/media/sanandreas.jpg", this);
    options[2] = new MapOption("Vice City", ":/media/vicecity.jpg", this);
    options[3] = new MapOption("Custom", ":/media/city.jpeg", this);

    QHBoxLayout* box = new QHBoxLayout;
    for (int i = 0; i < 4; i++) {
        box->addWidget(options[i]);
        connect(options[i], &MapOption::clicked,
                this, &Menu::onOptionClicked);
    }

    ui->optionsContainer->setLayout(box);

    connect(ui->selectButton, &QPushButton::clicked,
            this, &Menu::onSelectButtonPressed);
}

void Menu::on_buttonExit_clicked() { emit exitClicked(); }

void Menu::onOptionClicked(MapOption* opt) {
    if (current)
        current->setSelected(false);

    current = opt;
    current->setSelected(true);
}

void Menu::onSelectButtonPressed() {
    if (!current) {
        std::cout << "Error, Please select a map." << std::endl;
        return;
    }

    int selectedCityId = 1;

    emit mapSelected(selectedCityId);
}

Menu::~Menu() { delete ui; }
