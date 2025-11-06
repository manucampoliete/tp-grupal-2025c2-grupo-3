target_sources(taller_common
    PRIVATE
    # .cpp files
    PUBLIC
    # .h files
    )

add_subdirectory(errors)
add_subdirectory(commands)
add_subdirectory(protocol)
add_subdirectory(queue)
add_subdirectory(socket)
add_subdirectory(thread)
add_subdirectory(utils)
add_subdirectory(messages)
