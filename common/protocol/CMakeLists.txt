target_sources(taller_common
    PRIVATE
    # .cpp files
    dummy_client_protocol.cpp
    dummy_server_protocol.cpp
    PUBLIC
    # .h files
    protocol_constants.h
    dummy_client_protocol.h
    dummy_server_protocol.h
    game_data.h
    )