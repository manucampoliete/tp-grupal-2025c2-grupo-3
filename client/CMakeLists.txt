target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    client.cpp
    receiver.cpp
    sender.cpp
    game.cpp
    worldRenderer.cpp
    interfaceRenderer.cpp
    world.cpp
    eventHandler.cpp
    gameHandler.cpp
    soundManager.cpp
    gameLoop.cpp
    PUBLIC
    # .h files
    client.h
    receiver.h
    sender.h
    game.h
    worldRenderer.h
    interfaceRenderer.h
    world.h
    eventHandler.h
    gameHandler.h
    soundManager.h
    gameLoop.h
    )

add_subdirectory(protocol)