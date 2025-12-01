target_sources(needForSpeed2DServer
    PRIVATE
    # .cpp files
    main.cpp
    server.cpp
    configLoader.cpp
    PUBLIC
    # .h files
    configLoader.h
    server.h
    )
    
add_subdirectory(clientHandling)
add_subdirectory(commands)
add_subdirectory(gameLogic)
add_subdirectory(protocol)
add_subdirectory(requestsResolving)
add_subdirectory(synchronized)
