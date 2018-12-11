#ifndef GET_NUM_HPP
#define GET_NUM_HPP

enum {
  GN_NONNEG = 01,
  GN_GT_0 = 02,
  GN_ANY_BASE = 0100,
  GN_BASE_8 = 0200,
  GN_BASE_16 = 0400
};

long getLong(const char *arg, int flags, const char *name);
int getInt(const char *arg, int flags, const char *name);

#endif
