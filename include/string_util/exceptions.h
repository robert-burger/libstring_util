/*
    This file is part of links and nodes.

    links and nodes is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    links and nodes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with links and nodes.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2013 DLR e.V., Florian Schmidt, Maxime Chalon
*/
#ifndef STRING_UTIL_EXCEPTIONS_H
#define STRING_UTIL_EXCEPTIONS_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <exception>
#include <string>

class errno_exception : public std::exception {
  char msg[512];
  
 public:
  errno_exception(const char* format, ...);
  errno_exception(int retval, const char* format, ...);

  virtual const char* what() const throw() { return msg; }
};

class str_exception : public std::exception {
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

