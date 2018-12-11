#ifndef ERROR_FUNCTIONS_HPP
#define ERROR_FUNCTIONS_HPP

#include <cstdarg>

void errMsg(const char *format, ...);
[[noreturn]] void errExit(const char *format, ...);
[[noreturn]] void err_exit(const char *format, ...);
[[noreturn]] void errExitEN(int errnum, const char *format, ...);
[[noreturn]] void fatal(const char *format, ...);
[[noreturn]] void usageErr(const char *format, ...);
[[noreturn]] void cmdLineErr(const char *format, ...);

#endif
