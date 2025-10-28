target_sources(taller_client
    PRIVATE
    # .cpp files
    client.cpp
    main.cpp
    receiver.cpp
    sender.cpp
    game.cpp
    car.cpp
    world.cpp
    event_handler.cpp
    PUBLIC
    # .h files
    client.h
    receiver.h
    sender.h
    game.h
    car.h
    world.h
    event_handler.h
    )
