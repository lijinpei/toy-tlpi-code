#include <cstdio>
#include <cstdlib>

extern char **environ;

int main() {
  for (char** ep = environ; *ep; ++ep) {
    puts(*ep);
  }
  exit(EXIT_SUCCESS);
}
