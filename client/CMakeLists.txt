target_sources(taller_client
    PRIVATE
    # .cpp files
    client.cpp
    main.cpp
    client_protocol.cpp
    receiver.cpp
    sender.cpp
    game.cpp
    world_renderer.cpp
    ui_renderer.cpp
    car.cpp
    world.cpp
    event_handler.cpp
    PUBLIC
    # .h files
    client.h
    client_protocol.h
    receiver.h
    sender.h
    game.h
    world_renderer.h
    ui_renderer.h
    car.h
    world.h
    event_handler.h
    )
