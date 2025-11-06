target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    receiver.cpp
    sender.cpp
    game.cpp
    world_renderer.cpp
    interface_renderer.cpp
    car.cpp
    world.cpp
    event_handler.cpp
    game_handler.cpp
    PUBLIC
    # .h files
    client.h
    receiver.h
    sender.h
    game.h
    world_renderer.h
    interface_renderer.h
    car.h
    world.h
    event_handler.h
    game_handler.h
    )

add_subdirectory(protocol)
