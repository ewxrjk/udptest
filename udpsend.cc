#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

static void fatal(const char *s, ...) {
  int e = errno;
  va_list ap;
  va_start(ap, s);
  vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, " (errno=%s)\n", strerror(e));
  exit(1);
}

int main(int argc, char **argv) {
  if(argc != 4)
    fatal("usage: udpsend HOST PORT MESSAGE");
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = 0;
  int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
  if(rc)
    fatal("getaddrinfo: %s", gai_strerror(rc));
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(fd < 0)
    fatal("socket");
  if(connect(fd, res->ai_addr, res->ai_addrlen) < 0)
    fatal("connect");
  if(write(fd, argv[3], strlen(argv[3]) + 1) < 0)
    fatal("write");
  return 0;
}
