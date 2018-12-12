#include <signal.h>
#include "tlpi_hdr.hpp"

namespace {
  void sigHandler(int) {
    printf("Ouch!\n");
  }
}

int main() {
  if (signal(SIGINT, sigHandler) == SIG_ERR) {
    errExit("signal");
  }
  for (int j = 0; ; ++j) {
    printf("%d\n", j);
    sleep(3);
  }
}
