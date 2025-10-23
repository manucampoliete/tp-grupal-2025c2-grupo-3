target_sources(taller_common
    PRIVATE
    # .cpp files
    socket/liberror.cpp
    socket/resolver.cpp
    socket/resolvererror.cpp
    socket/socket.cpp
    thread/thread.cpp
    PUBLIC
    # .h files
    socket/liberror.h
    queue/queue.h
    socket/resolver.h
    socket/resolvererror.h
    thread/runnable.h
    socket/socket.h
    thread/thread.h
    )
