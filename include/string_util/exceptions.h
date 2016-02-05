#ifndef STRING_UTIL_EXCEPTIONS_H
#define STRING_UTIL_EXCEPTIONS_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <exception>
#include <string>

using namespace std;

class errno_exception : public std::exception {
  char msg[512];
  
 public:
  errno_exception(const char* format, ...);
  errno_exception(int retval, const char* format, ...);

  virtual const char* what() const throw() { return msg; }
};

class str_exception : public exception {
  char msg[1024];
  
 public:
  str_exception(const char* format, ...);

  virtual const char* what() const throw() { return msg; }
};

#define str_exception_tb(format, ...) str_exception("%s:%d %s()\n   " format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define errno_exception_tb(format, ...) errno_exception("%s:%d %s()\n   " format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define retval_exception_tb(retval, format, ...) errno_exception(retval, "%s:%d %s()\n   " format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define retval_exception(retval, format, ...) errno_exception(retval, format, ##__VA_ARGS__);

#endif // STRING_UTIL_EXCEPTIONS_H

