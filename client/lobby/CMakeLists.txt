cmake_minimum_required(VERSION 3.24)

project(lobby LANGUAGES CXX)

# --- Configuración automática de Qt ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Buscar Qt6 Widgets ---
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# --- Fuentes de la lobby ---
target_sources(taller_client
    PRIVATE
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        mainmenu.cpp
        mainmenu.h
        mainmenu.ui

        joingame.cpp
        joingame.h
        joingame.ui

        newgame.cpp
        newgame.h
        newgame.ui

        hostwaiting.cpp
        hostwaiting.h
        hostwaiting.ui

        guestwaiting.cpp
        guestwaiting.h
        guestwaiting.ui

        carselector.cpp
        carselector.h

        carcard.cpp
        carcard.h

        resources.qrc
)

# --- Incluir directorio actual (para los headers locales) ---
target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Enlazar con Qt ---
target_link_libraries(taller_client
    PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
)