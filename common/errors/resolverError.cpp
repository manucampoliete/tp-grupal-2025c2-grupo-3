#include "resolverError.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

ResolverError::ResolverError(int gaiErrno): gaiErrno(gaiErrno) {}

const char* ResolverError::what() const noexcept { return gai_strerror(gaiErrno); }

ResolverError::~ResolverError() {}
