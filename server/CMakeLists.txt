target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    client_handler.cpp
    game.cpp
    main.cpp
    receiver.cpp
    response_queues_monitor.cpp
    sender.cpp
    server.cpp
    car.cpp
    player.cpp
    PUBLIC
    # .h files
    acceptor.h
    client_handler.h
    game.h
    receiver.h
    response_queues_monitor.h
    sender.h
    server.h
    types.h
    car.h
    player.h
    matches_map_monitor.h
    )

add_subdirectory(commands)
