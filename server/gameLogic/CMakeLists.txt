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
    collision_loader.h
    car_builder.h
    contact_listener.h
    bodyData.h
    )
