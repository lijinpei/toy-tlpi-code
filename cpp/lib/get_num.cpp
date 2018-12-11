#include "get_num.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include "errno.h"

namespace {
  void gnFail(const char *fname, const char *msg, const char *arg, const char *name) {
    fprintf(stderr, "%s error", fname);
    if (name) {
      fprintf(stderr, " (in %s)", name);
    }
    fprintf(stderr, ": %s\n", msg);
    if (arg && *arg) {
      fprintf(stderr, "        offending test: %s\n", arg);
    }
    exit(EXIT_FAILURE);
  }

  long getNum(const char *fname, const char *arg, int flags, const char *name) {
    if (!arg || !*arg) {
      gnFail(fname, "null or empty string", arg, name);
    }
    int base = (flags & GN_ANY_BASE) ? 0 :
      (flags & GN_BASE_8) ? 8 :
      (flags & GN_BASE_16) ? 16 :
      10;
    errno = 0;
    char *endptr;
    long res = strtol(arg, &endptr, base);
    if (errno) {
      gnFail(fname, "strtol() failed", arg, name);
    }
    if (*endptr) {
      gnFail(fname, "nonnumeric characters", arg, name);
    }
    if ((flags & GN_NONNEG) && res < 0) {
      gnFail(fname, "negative value not allowed", arg, name);
    }
    if ((flags & GN_GT_0) && res <= 0) {
      gnFail(fname, "value must be > 0", arg, name);
    }
    return res;
  }
}

long getLong(const char *arg, int flags, const char *name) {
  return getNum("getLong", arg, flags, name);
}

int getInt(const char *arg, int flags, const char *name) {
  long res = getNum("getInt", arg, flags, name);
  if (res > INT_MAX || res < INT_MIN) {
    gnFail("getInt", "integer out of range", arg, name);
  }
  return int(res);
}
