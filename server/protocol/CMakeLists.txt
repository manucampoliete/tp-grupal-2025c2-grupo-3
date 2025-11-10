target_sources(taller_server
    PRIVATE
    # .cpp files
    serverLobbyProtocol.cpp
    serverGameRecvProtocol.cpp
    serverGameSendProtocol.cpp
    PUBLIC
    # .h files
    serverLobbyProtocol.h
    serverGameRecvProtocol.h
    serverGameSendProtocol.h
    )
