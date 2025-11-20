target_sources(taller_server
    PRIVATE
    # .cpp files
    game.cpp
    player.cpp
    car.cpp
    contact_listener.cpp
    PUBLIC
    # .h files
    game.h
    player.h
    car.h
    car_builder.h
    contact_listener.h
    )
