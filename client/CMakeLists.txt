target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    client.cpp
    receiver.cpp
    sender.cpp
    game.cpp
    world_renderer.cpp
    interface_renderer.cpp
    world.cpp
    event_handler.cpp
    game_handler.cpp
    sound_manager.cpp
    PUBLIC
    # .h files
    client.h
    receiver.h
    sender.h
    game.h
    world_renderer.h
    interface_renderer.h
    world.h
    event_handler.h
    game_handler.h
    sound_manager.h
    )

add_subdirectory(protocol)