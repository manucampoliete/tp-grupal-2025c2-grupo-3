cmake_minimum_required(VERSION 3.24)

target_sources(needForSpeed2DEditor
    PRIVATE
        editor.h editor.cpp editor.ui
        menu.h menu.cpp menu.ui
        main.cpp
        editor_resources.qrc
        mapoption.h mapoption.cpp
        mapeditor.h mapeditor.cpp
        mapview.h mapview.cpp
        toolbox.h toolbox.cpp
        mapelement.h mapelement.cpp
        elementicon.h elementicon.cpp
        circuitsegment.h
    )

target_include_directories(needForSpeed2DEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
