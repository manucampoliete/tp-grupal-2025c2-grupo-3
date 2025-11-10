target_sources(taller_server
    PRIVATE
    # .cpp files
    game.cpp
    player.cpp
    PUBLIC
    # .h files
    game.h
    player.h
    )

add_subdirectory(cars)
