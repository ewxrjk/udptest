#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>

static int portno = 4096;

static void fatal(const char *s, ...) {
  int e = errno;
  va_list ap;
  va_start(ap, s);
  vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, " (errno=%s)\n", strerror(e));
  exit(1);
}

int bindto(const char *dev) {
  struct sockaddr_in addr;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(fd < 0) fatal("socket");
  const int one = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0)
    fatal("setsockopt SO_REUSEADDR");
  if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof one) < 0)
    fatal("setsockopt SO_BROADCAST");
  if(dev && *dev) {
#ifdef SO_BINDTODEVICE
    if(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, dev, strlen(dev) + 1) < 0)
      fatal("setsockopt SO_BINDTODEVICE %s", dev);
#else
    fatal("SO_BINDTODEVICE not implemented on this platform");
#endif
  }
  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portno);
  addr.sin_addr.s_addr = 0;
  if(bind(fd, (struct sockaddr *)&addr, sizeof addr) < 0)
    fatal("bind");
  return fd;
}

int main(int argc, char **argv) {
  char buffer[4096];
  int fds[64];
  if(argc < 2)
    fatal("Usage: udplisten DEVICE [DEVICE...]");
  for(int n = 1; n < argc; ++n)
    fds[n] = bindto(argv[n]);
  for(;;) {
    fd_set fdset;
    FD_ZERO(&fdset);
    for(int n = 1; n < argc; ++n)
      FD_SET(fds[n], &fdset);
    int rc = select(fds[argc-1] + 1, &fdset, NULL, NULL, NULL);
    if(rc < 0)
      fatal("select");
    for(int n = 1; n < argc; ++n) {
      if(FD_ISSET(fds[n], &fdset)) {
        int len = read(fds[n], buffer, sizeof buffer - 1);
        if(len < 0)
          fatal("recv (%s)", argv[n]);
        buffer[len] = 0;
        printf("%s: %s\n", argv[n], buffer);
        fflush(stdout);
      }
    }
  }
}
