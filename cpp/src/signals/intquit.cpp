#include <signal.h>
#include "tlpi_hdr.hpp"

namespace {
  void sigHandler(int sig) {
    static int count = 0;
    if (sig == SIGINT) {
      ++count;
      /* UNSAFE */
      printf("Caught SIGINT (%d)\n", count);
      return;
    }
    printf("Caught SIGQUIT - that's all folks!\n");
    /* UNSAFE */
    exit(EXIT_SUCCESS);
  }
}

int main() {
  if (signal(SIGINT, sigHandler) == SIG_ERR || (signal(SIGQUIT, sigHandler) == SIG_ERR)) {
    errExit("signal");
  }
  while (true) {
    pause();
  }
}
