target_sources(needForSpeed2DServer
    PRIVATE
    # .cpp files
    game.cpp
    player.cpp
    car.cpp
    PUBLIC
    # .h files
    game.h
    player.h
    car.h
    carBuilder.h
    bodyData.h
    )

add_subdirectory(collisions)