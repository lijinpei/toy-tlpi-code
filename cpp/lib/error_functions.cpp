#include "error_functions.hpp"
#include "tlpi_hdr.hpp"

#include <cstdarg>

#include "ename.generated.inc"

using namespace std;

namespace {

[[noreturn]] void terminate(bool useExit3) {
  const char *s = getenv("EF_DUMPCORE");
  if (s && *s) {
    abort();
  } else if (useExit3) {
    exit(EXIT_FAILURE);
  } else {
    _exit(EXIT_FAILURE);
  }
}

void outputError(bool useErr, int err, bool flushStdout, const char *format, va_list ap) {
  if (flushStdout) {
    fflush(stdout);
  }
  if (useErr) {
    fprintf(stderr, "ERROR [%s %s] ", (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?", strerror(err));
  } else {
    fprintf(stderr, "ERROR: ");
  }
  vfprintf(stderr, format, ap);
  fflush(stderr);
}
/*
template <bool useErr, bool flushStdout>
void errExitBuilder(int errno_, const char *format, va_list argList) {
  outputError(useErr, errno_, flushStdout, format, argList);
  terminate(flushStdout);
}
*/
/*
template <const char* message>
void usageErrorBuilder(const char *format, ...) {
  va_list argList;
  fflush(stdout);
  fprintf(stderr, message);
  va_start(argList, format);
  vfprintf(stderr, format, argList);
  va_end(argList);

  fflush(stderr);
  exit(EXIT_FAILURE);
}
*/
}

void errMsg(const char *format, ...) {
  int savedErrno = errno;
  va_list argList;
  va_start(argList, format);
  outputError(true, errno, true, format, argList);
  va_end(argList);
  errno = savedErrno;
}

void errExit(const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  outputError(true, errno, true, format, argList);
  va_end(argList);
  terminate(true);
  /*
  va_list argList;
  va_start(argList, format);
  errExitBuilder<true, true>(errno, format, argList);
  va_end(argList);
  */
}

void err_exit(const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  outputError(true, errno, false, format, argList);
  va_end(argList);
  terminate(false);
  /*
  va_list argList;
  va_start(argList, format);
  errExitBuilder<true, false>(errno, format, argList);
  va_end(argList);
  */
}

void errExitEN(int errnum, const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  outputError(true, errnum, true, format, argList);
  va_end(argList);
  terminate(true);
  /*
  errExitBuilder<true, false>(errnum, format, ...);
  */
}

void fatal(const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  outputError(false, 0, true, format, argList);
  terminate(true);
  /*
  errExitBuilder<false, true>(errno, format, ...);
  */
}

void usageErr(const char *format, ...) {
  va_list argList;
  fflush(stdout);
  fprintf(stderr, "Usage: ");
  va_start(argList, format);
  vfprintf(stderr, format, argList);
  va_end(argList);

  fflush(stderr);
  exit(EXIT_FAILURE);
  /*
  const char* const message = "Usage: ";
  usageErrorBuilder<message>(format, ...);
  */
}

void cmdLineErr(const char *format, ...) {
  va_list argList;
  fflush(stdout);
  fprintf(stderr, "Command-line usage error: ");
  va_start(argList, format);
  vfprintf(stderr, format, argList);
  va_end(argList);

  fflush(stderr);
  exit(EXIT_FAILURE);
  /*
  const char* const message = "Command-line usage error: ";
  usageErrorBuilder<message>(format, ...);
  */
}
