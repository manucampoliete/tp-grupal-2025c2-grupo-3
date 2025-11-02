cmake_minimum_required(VERSION 3.24)

target_sources(taller_client
    PRIVATE
        lobby.cpp
        lobby.h
        lobby.ui

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

target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})