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
#ifndef STRING_UTIL_OS_H
#define STRING_UTIL_OS_H

#ifndef NO_CONFIG_H
#include "config.h"
#endif

#ifdef __FUNCTION__
#define __func__ __FUNCTION__
#endif

#ifndef HAVE_SNPRINTF
int string_util_vsnprintf(char* str, size_t size, const char* format, va_list ap);
int string_util_snprintf(char* str, size_t size, const char* format, ...);

#define snprintf string_util_snprintf
#define vsnprintf string_util_vsnprintf
#endif

#endif
