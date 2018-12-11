#include <vector>
#include "tlpi_hdr.hpp"

#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, const char *argv[]) {
  if (argc < 3 || !strcmp(argv[1], "--help")) {
    usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}... \n",
             argv[0]);
  }
  int fd = open(argv[1], O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH |
                    S_IWOTH); /* rw-rw-rw- */
  if (fd == -1) {
    errExit("open");
  }
  std::vector<char> buf;
  for (int ap = 2; ap < argc; ++ap) {
    switch (argv[ap][0]) {
      case 'r': /* Display bytes at current offset, as text */
      case 'R': /* Display bytes at current offset, in hex */ {
        long len = getLong(argv[ap] + 1, GN_ANY_BASE, argv[ap]);
        buf.resize(len);
        ssize_t numRead = read(fd, buf.data(), len);
        if (numRead == -1) {
          errExit("read");
        }
        if (numRead == 0) {
          printf("%s: end-of-file\n", argv[ap]);
        } else {
          printf("%s: ", argv[ap]);
          if (argv[ap][0] == 'r') {
            for (int j = 0; j < numRead; ++j) {
              printf("%c", isprint(buf[j]) ? buf[j] : '?');
            }
          } else {
            for (int j = 0; j < numRead; ++j) {
              printf("%02x ", (unsigned int)buf[j]);
            }
          }
        }
      }
        break;
      case 'w': /* write string at current offset */ {
        int numWrite = write(fd, argv[ap] + 1, strlen(argv[ap] + 1));
        if (numWrite == -1) {
          errExit("write");
        }
        printf("%s: wrote %ld bytes\n", argv[ap], (long)numWrite);
      }
        break;
      case 's': /* change file offset */ {
        long offset = getLong(argv[ap] + 1, GN_ANY_BASE, argv[ap]);
        if (lseek(fd, offset, SEEK_SET) == -1) {
          errExit("lseek");
        }
        printf("%s: seek succeeded\n", argv[ap]);
      }
        break;
      default:
        cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
    }
  }
  exit(EXIT_SUCCESS);
}
