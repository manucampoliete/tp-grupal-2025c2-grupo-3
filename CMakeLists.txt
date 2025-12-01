cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS ON)
SET(SDL2PP_WITH_IMAGE YES)
SET(SDL2PP_WITH_TTF YES)
SET(SDL2PP_WITH_MIXER YES)

project(NeedForSpeed
    VERSION 1.0
    HOMEPAGE_URL "https://github.com/manucampoliete/tp-grupal-2025c2-grupo-3"
    LANGUAGES CXX
    )

option(NEED_FOR_SPEED_2D_TESTS "Enable / disable tests." OFF)
option(NEED_FOR_SPEED_2D_CLIENT "Enable / disable client program." ON)
option(NEED_FOR_SPEED_2D_SERVER "Enable / disable server program." ON)
option(NEED_FOR_SPEED_2D_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
add_library(needForSpeed2DCommon STATIC)

# add the source files that make the lib
add_subdirectory(common/)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(needForSpeed2DCommon ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

# Say where the headers will be found required by library target
# Make this PUBLIC so any dependant linking lib will inherit this
# location (so our dependants will not have to explicitly request this)
target_include_directories(needForSpeed2DCommon PUBLIC .)

# Define INSTALL_MODE if requested
if(DEFINED INSTALL_MODE AND INSTALL_MODE)
    add_compile_definitions(INSTALL_MODE)
endif()

# Install libSDL2 and libSDL2pp (for client or editor)
if(NEED_FOR_SPEED_2D_CLIENT OR NEED_FOR_SPEED_2D_EDITOR)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-2.8.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-2.22.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        libSDL2pp
        OVERRIDE_FIND_PACKAGE
        URL https://github.com/libSDL2pp/libSDL2pp/archive/cc198c9a5657048bee67ece82de620b2d5661084.zip
    )

    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf libSDL2pp)
endif()

# Program section
# ---------------
if(NEED_FOR_SPEED_2D_CLIENT)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    add_executable(needForSpeed2DClient)

    # Make them depend on needForSpeed2DCommon lib
    add_dependencies(needForSpeed2DClient needForSpeed2DCommon SDL2pp::SDL2pp)

    # Add source files and enable warnings
    add_subdirectory(client)

    find_package(SDL2_mixer REQUIRED)

    find_package(Qt6 COMPONENTS Widgets Multimedia Core Gui REQUIRED)
    add_subdirectory(client/lobby)

    set_project_warnings(needForSpeed2DClient ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(needForSpeed2DClient PRIVATE
        ${libSDL2pp_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/client/lobby
    )

    # Add yaml-cpp
    find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
    find_library(YAMLCPP_LIBRARY yaml-cpp)
    target_include_directories(needForSpeed2DClient PRIVATE ${YAMLCPP_INCLUDE_DIR})

    # Link the needForSpeed2DCommon lib target
    target_link_libraries(needForSpeed2DClient
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        needForSpeed2DCommon
        SDL2pp::SDL2pp
        SDL2_mixer::SDL2_mixer
        ${YAMLCPP_LIBRARY}
    )

endif()

if(NEED_FOR_SPEED_2D_SERVER)
    add_executable(needForSpeed2DServer)

    # Make them depend on needForSpeed2DCommon lib
    add_dependencies(needForSpeed2DServer needForSpeed2DCommon)

    # Add source files and enable warnings
    add_subdirectory(server)

    set_project_warnings(needForSpeed2DServer ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # Add Box2D
    find_path(BOX2D_INCLUDE_DIR box2d/box2d.h)
    find_library(BOX2D_LIBRARY box2d)

    if(BOX2D_INCLUDE_DIR AND BOX2D_LIBRARY)
        target_include_directories(needForSpeed2DServer PRIVATE ${BOX2D_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "Box2D not found")
    endif()

    # Add yaml-cpp
    find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
    find_library(YAMLCPP_LIBRARY yaml-cpp)

    if(YAMLCPP_INCLUDE_DIR AND YAMLCPP_LIBRARY)
        target_include_directories(needForSpeed2DServer PRIVATE ${YAMLCPP_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "yaml-cpp not found")
    endif()

    # Link the needForSpeed2DCommon lib target + box2d + yaml-cpp
    target_link_libraries(needForSpeed2DServer needForSpeed2DCommon ${BOX2D_LIBRARY} ${YAMLCPP_LIBRARY})
endif()

if(NEED_FOR_SPEED_2D_EDITOR)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    find_package(yaml-cpp CONFIG REQUIRED)

    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

    add_executable(needForSpeed2DEditor)

    # Make them depend on needForSpeed2DCommon lib
    add_dependencies(needForSpeed2DEditor needForSpeed2DCommon SDL2pp::SDL2pp)

    # Add source files and enable warnings
    add_subdirectory(editor)

    set_project_warnings(needForSpeed2DEditor ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(needForSpeed2DEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/editor)

    # Link the needForSpeed2DCommon lib target
    target_link_libraries(needForSpeed2DEditor
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        yaml-cpp::yaml-cpp
    )

endif()


# Testing section
# ---------------

if(NEED_FOR_SPEED_2D_TESTS)
    # Let's use google test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    # (borrowed from https://google.github.io/googletest/quickstart-cmake.html)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    # target for testing
    add_executable(needForSpeed2DTests)

    # HEY!! TODO XXX: I'm assuming that the code under testing is in needForSpeed2DCommon
    # Change this if you have to.
    add_dependencies(needForSpeed2DTests needForSpeed2DCommon)

    # Say where to find the xoz headers
    # HEY!! TODO XXX: add more subfolders here
    target_include_directories(needForSpeed2DTests PUBLIC .)

    # Add the source files to the needForSpeed2DTests target
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(tests/)

    # Set the compiler warnings to the tests target (but be a little lesser strict)
    set_project_warnings(needForSpeed2DTests ${TALLER_MAKE_WARNINGS_AS_ERRORS} TRUE)

    # Say where googletest headers can be found
    target_include_directories(needForSpeed2DTests
        PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )

    # HEY!! TODO XXX: I'm assuming that the code under testing is in needForSpeed2DCommon
    # Change this if you have to.
    target_link_libraries(needForSpeed2DTests
        needForSpeed2DCommon
        GTest::gtest_main
    )
endif()