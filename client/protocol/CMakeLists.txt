target_sources(needForSpeed2DClient
    PRIVATE
    # .cpp files
    clientGameRecvProtocol.cpp
    clientGameSendProtocol.cpp
    clientHandshakeProtocol.cpp
    PUBLIC
    # .h files
    clientGameRecvProtocol.h
    clientGameSendProtocol.h
    clientHandshakeProtocol.h
    )
