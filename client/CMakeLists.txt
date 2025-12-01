target_sources(needForSpeed2DClient
    PRIVATE
    # .cpp files
    main.cpp
    client.cpp
    PUBLIC
    # .h files
    client.h
    )

add_subdirectory(audio)
add_subdirectory(gameHandling) 
add_subdirectory(protocol)
add_subdirectory(rendering)
add_subdirectory(threads)
