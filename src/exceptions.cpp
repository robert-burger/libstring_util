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

#ifndef NO_CONFIG_H
#include "config.h"
#else
#ifdef WIN32
#define HAVE_WINDOWS_H
#endif
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <string_util/string_util.h>

#include "os.h"

errno_exception::errno_exception(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, 1024, format, ap);
	va_end(ap);
	string stlmsg(msg);
#ifdef HAVE_WINDOWS_H
	{ 
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError(); 
		
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

		stlmsg = format_string(
			"%s\nGetLastError(): %d\nFormatMessage(): %s\nerrno",
			stlmsg.c_str(),
			dw,
			lpMsgBuf);

		LocalFree(lpMsgBuf);
	}
#endif
    if(stlmsg.size())
		snprintf(this->msg, 512, "%s: %s (errno %d)", stlmsg.c_str(), strerror(errno), errno);
    else
		snprintf(this->msg, 512, "%s (errno %d)", strerror(errno), errno);
}

errno_exception::errno_exception(int retval, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, 1024, format, ap);
	va_end(ap);
	string stlmsg(msg);
    if(stlmsg.size())
		snprintf(this->msg, 512, "%s: %s (retval %d)", stlmsg.c_str(), strerror(retval), retval);
    else
		snprintf(this->msg, 512, "%s (retval %d)", strerror(retval), retval);
}

str_exception::str_exception(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, 1024, format, ap);
	va_end(ap);
}
