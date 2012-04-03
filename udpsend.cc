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
  char *buffer;
  if(argc < 4 || argc > 5)
    fatal("usage: udpsend HOST PORT MESSAGE [REPEATS]");
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_V4MAPPED|AI_ADDRCONFIG;
  int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
  if(rc)
    fatal("getaddrinfo: %s", gai_strerror(rc));
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(fd < 0)
    fatal("socket");
  const int one = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof one) < 0)
    fatal("setsockopt SO_BROADCAST");
  int repeats = 1;
  if(argc > 4)
    repeats = atoi(argv[4]);
  buffer = new char[strlen(argv[3]) + 10];
  for(int n = 0; n < repeats; ++n) {
    sprintf(buffer, "%s %d", argv[3], n);
    if(sendto(fd, buffer, strlen(buffer) + 1, 0,
	      res->ai_addr, res->ai_addrlen) < 0)
      fatal("sendto[%d]", n);
  }
  return 0;
}
