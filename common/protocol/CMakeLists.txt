target_sources(taller_common
    PRIVATE
    # .cpp files
    dummy_server_protocol.cpp
    client_protocol.cpp
    PUBLIC
    # .h files
    protocol_constants.h
    dummy_server_protocol.h
    game_data.h
    client_protocol.h
    )