#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>

#include "tlpi_hdr.hpp"

int main(int argc, const char *argv[]) {
  const int STR_SIZE = 100;
  char str[STR_SIZE];
  if (argc != 2 || !strcmp(argv[1], "--help")) {
    usageErr("%s file\n", argv[0]);
  }
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }
  int totRequired = 0;
  iovec iov[3];
  struct stat myStruct;
  int x;

  iov[0].iov_base = &myStruct;
  iov[0].iov_len = sizeof(struct stat);
  totRequired += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len = sizeof(x);
  totRequired += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len = STR_SIZE;
  totRequired += iov[2].iov_len;

  int numRead = readv(fd, iov, 3);
  if (numRead == -1) {
    errExit("readv");
  }

  if (numRead < totRequired) {
    printf("Read fewer bytes than requested\n");
  }
  printf("total bytes requested: %ld; bytes read: %ld\n", (long) totRequired, (long) numRead);
  exit(EXIT_SUCCESS);
}
