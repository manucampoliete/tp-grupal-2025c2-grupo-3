target_sources(taller_common
    PRIVATE
    # .cpp files
    recvProtocol.cpp
    sendProtocol.cpp
    client_protocol.cpp
    PUBLIC
    # .h files
    recvProtocol.h
    sendProtocol.h
    protocolConstants.h
    protocol_constants.h
    game_data.h
    client_protocol.h
    )