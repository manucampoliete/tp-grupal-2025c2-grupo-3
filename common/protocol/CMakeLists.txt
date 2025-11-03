target_sources(taller_common
    PRIVATE
    # .cpp files
    client_protocol.cpp
    server_protocol.cpp
    PUBLIC
    # .h files
    client_protocol.h
    game_consumer.h
    lobby_consumer.h
    protocol_constants.h
    protocol_consumer.h
    server_protocol.h
    )