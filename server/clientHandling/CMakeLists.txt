target_sources(taller_server
    PRIVATE
    # .cpp files
    acceptor.cpp
    clientHandler.cpp
    receiver.cpp
    sender.cpp
    PUBLIC
    # .h files
    acceptor.h
    clientHandler.h
    receiver.h
    sender.h
    )
