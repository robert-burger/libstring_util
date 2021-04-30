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

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <cstdio>
#include <sstream>

#include <string_util/string_util.h>

namespace string_util {

#include "os.h"

using namespace std;

string binary_data_repr(const char* input, int input_len) {
	stringstream ss;
	ss << "data ";
	ss << input_len;
	ss << "\n";
	ss.write(input, input_len);
	return ss.str();
}
string binary_data_repr(const string& input) {
	return binary_data_repr(input.c_str(), input.size());
}

string repr(const char* input, bool use_dbl_quote) {
	return repr(input, strlen(input), use_dbl_quote);
}
string repr(const char* input, int input_len, bool use_dbl_quote) {
	stringstream ss;
	if(!use_dbl_quote)
		ss << "'";
	else
		ss << "\"";
	for(const char* cp = input; cp != input + input_len; cp++) {
		if((strchr(" ,.*:;/_-+=[](){}^!?$", *cp) || isdigit((unsigned char)*cp) || isalpha((unsigned char)*cp)) && (unsigned char)*cp <= 127 && *cp) {
			//log("printable char: %d %c", *cp, *cp);
			ss << *cp;
			continue;
		}
		if(*cp == '\r') ss << "\\r";
		else if(*cp == '\n') ss << "\\n";
		else if(*cp == '<') ss << "<";
		else if(*cp == '>') ss << ">";
		else if(*cp == '\t') ss << "\\t";
		else if(*cp == '\\') ss << "\\\\";
		else if(!use_dbl_quote && *cp == '\'') ss << "\\'";
		else if(!use_dbl_quote && *cp == '"') ss << "\"";
		else if(*cp == '|') ss << "|";
		else if(*cp == '%') ss << "%";
		else {
			char format[16];
			snprintf(format, 16, "\\x%02x", (unsigned char)*cp);
			ss << format;
		}
	}	
	if(!use_dbl_quote)
		ss << "'";
	else
		ss << "\"";
	return ss.str();
}

string repr(string input, bool use_dbl_quote) {
	return repr(input.c_str(), input.size(), use_dbl_quote);
}

string repr(py_value* input, bool use_dbl_quote) {
	return input->repr();
	
	py_dict* d = dynamic_cast<py_dict*>(input);
	if(d)
		return /*string("dict:") + */string(*d);
	py_list* l = dynamic_cast<py_list*>(input);
	if(l)
		return /*string("list:") + */string(*l);
	py_int* i = dynamic_cast<py_int*>(input);
	if(i)
		return /*string("int:") + */string(*i);
	py_long* li = dynamic_cast<py_long*>(input);
	if(li)
		return /*string("long:") + */string(*li);
	py_float* f = dynamic_cast<py_float*>(input);
	if(f)
		return /*string("int:") + */string(*f);
	py_special* s = dynamic_cast<py_special*>(input);
	if(s)
		return /*string("special:") + */string(*s);
	return /*string("other:") + */repr(string(*input), use_dbl_quote);

}

string repr(py_value& input) {
	return input.repr();
}

string repr(double input) {
	return rstrip(format_string("%.16f", input), "0");
}
string repr(float input) {
	return rstrip(format_string("%.16f", input), "0");
}
string repr(uint32_t input) {
	return format_string("%u", input);
}
string repr(int32_t input) {
	return format_string("%d", input);
}
string repr(uint64_t input) {
	return format_string("%lluL", input);
}
string repr(int64_t input) {
	return format_string("%lldL", input);
}
string repr(bool input) {
	if(input)
		return "True";
	return "False";
}


unsigned int hex2dec(string hex) {
#if !defined(__QNX__) && !defined(__VXWORKS__) && !defined(ARM)
	// strchr(char*, char);
	const
#endif		
		char* hex_order = "0123456789abcdef"; // can not be const because strchr on qnx expects non-const!
	unsigned int value = 0;
	unsigned int base = 1;
	for(string::size_type p = hex.size() - 1; ; p--) {
		char c = hex[p];
		char* cp = (char*)strchr(hex_order, c);
		if(!cp)
			throw str_exception_tb("invalid hex char '%c' in %s at pos %d", c, repr(hex).c_str(), p);
		int index = cp - hex_order;
		value += index * base;
		base *= 16;
		if(!p)
			return value;
	}
}

string eval(string value) {
	string output;
	eval_string_until(value, 0, output);
	return output;
}

string::size_type eval_string_until(string value, string::size_type start, string& output) {
	stringstream ss;
	string::size_type p = start;
	string::size_type tp;

	p = value.find_first_not_of(" \r\n\t", p);
	if(p == string::npos)
		throw str_exception_tb("only whitespaces found!");

	// printf("try to read string from %s - start: %d\n", repr(value.substr(p)).c_str(), start);
	char delim = value[p];
	if(delim != '\'' && delim != '"') {
		// no delim!
		// printf("no delim!\n");
		// search next non identifier char
		tp = value.find_first_of(" \r\n\t,:'\"}{[]()", p);
		output = value.substr(p, tp - p);
		// printf("got non delim string %s\n", repr(output).c_str());
		return tp - 1;
		// throw str_exception_tb("can not eval non-string value %s - not supported start delim %c!", 
		//repr(value).c_str(), delim);
	}
	p++;
	while(true) {
		if(p >= value.size()) {
			tp = value.size() - 1;
			break;
		}
		// find next backslash
		// printf("find backslash starting from %s\n", value.substr(p).c_str());
		string::size_type np = value.find('\\', p);
		if(np == string::npos) {
			// no further backslashes!
			// printf("no further backslashed!\n");
			// see where this string terminates!
			tp = value.find(delim, p);
			if(tp == string::npos)
				throw str_exception_tb("error: terminating ' not found in %s!", repr(value).c_str());			
			ss << value.substr(p, tp - p);
			break;
		}
		// found a backslash. 
		// printf("found a backslash\n");

		// check if there is a terminating '
		tp = value.find(delim, p);
		if(tp < np && tp != string::npos) {
			// found terminating tick before backslash! - finish
			// printf("found terminating tick before before next backslash. adding this string: %s\n", value.substr(p, tp - p - 1).c_str());
			ss << value.substr(p, tp - p);
			break;
		}
		// printf("handle backslash!\n");
			
		ss << value.substr(p, np - p);
		// look at next char
		if(value[np + 1] == 'r') ss << "\r";
		else if(value[np + 1] == 'n') ss << "\n";
		else if(value[np + 1] == '0') ss << "\0";
		else if(value[np + 1] == 't') ss << "\t";
		else if(value[np + 1] == '\\') ss << "\\";
		else if(value[np + 1] == '\'') ss << "'";
		else if(value[np + 1] == '"') ss << "\"";
		else if(value[np + 1] == 'x') {
			// 2 digit hex number
			ss << (char)hex2dec(value.substr(np + 2, 2));
			np += 2; // skip these two chars
		} else
			throw str_exception_tb("unknown excape sequence starting from %s", repr(value.substr(p)).c_str());
		p = np + 2;
	}
	output = ss.str();
	return tp;
}

dict eval_dict(string value) {
	//printf("reading dict: %s\n", value.c_str());
	dict o;
	string::size_type p = 1;

	if(value[0] != '{' || value[value.size() - 1] != '}')
		throw str_exception_tb("can not eval non-dict value %s!",
							repr(value).c_str());
	
	while(p < value.size() - 1) {
		string v_key;
		string v_value;
		// read key
		//printf("try to read key from %s\n", repr(value.substr(p)).c_str());
		string::size_type np = eval_string_until(value, p, v_key);
		// printf("got key %s and pos %d\n", repr(v_key).c_str(), np);
		if(np == string::npos) // no further key-value pairs!
			break;
		p = np + 2;
		// read value
		//printf("try to read value from %s\n", repr(value.substr(p)).c_str());
		np = eval_string_until(value, p, v_value);
		if(np == string::npos) // error reading value!
			break;
		o[v_key] = v_value;
		p = np + 2;
	}
	/*
	printf("got this dict:\n");
	for(dict::iterator i = o.begin(); i != o.end(); i++) {
		printf(" %s: %s\n", repr(i->first).c_str(), repr(i->second).c_str());
	}
	*/
	return o;
}
list<string> eval_list(string value) {
	// printf("reading list: %s\n", value.c_str());
	list<string> o;
	string::size_type p = 1;

	if(!strip(value).size())
		return o;
	if(value[0] != '[' || value[value.size() - 1] != ']')
		throw str_exception_tb("can not eval non-list value %s!",
							repr(value).c_str());
	
	while(p < value.size() - 1) {
		string v_key;
		//printf("try to read key from %s\n", repr(value.substr(p)).c_str());
		string::size_type np = eval_string_until(value, p, v_key);
		// printf("got key %s and pos %d\n", repr(v_key).c_str(), np);
		if(np == string::npos) // no further key-value pairs!
			break;
		o.push_back(v_key);
		p = np + 2;
	}
	/*
	printf("got this list:\n");
	for(list<string>::iterator i = o.begin(); i != o.end(); i++) {
		printf(" %s\n", repr(*i).c_str());
	}
	*/
	return o;
}

py_value* eval_full(string value, string::size_type start, string::size_type* np_out) {
	py_value* output = NULL;
	string::size_type p = start;
	string::size_type np;

	// printf("value: %s\n", value.c_str());
	p = value.find_first_not_of(" \r\n\t", p);
	if(p == string::npos)
		throw str_exception_tb("only whitespaces found!");

	if(value[p] == '{') {
		py_dict* d = new py_dict();
		output = d;
		p++;
		while(p < value.size()) { // todo: stop erlier!
			// read key
			//printf("try to read key from %s\n", repr(value.substr(p)).c_str());
			np = value.find_first_not_of(" \r\n\t", p);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted } in %s", repr(value).c_str());
			if(value[np] == '}')
				break;

			py_value* name = eval_full(value, p, &np);
			//printf("got key %s and pos %d\n", repr(*name).c_str(), np);
			if(np == string::npos)
				break;
			np = value.find_first_not_of(" \r\n\t", np + 1);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted : in %s", repr(value).c_str());
			if(value[np] != ':')
				throw str_exception_tb("expected : - got %c", value[np]);
			p = np + 1;
			// read value
			//printf("try to read value from %s\n", repr(value.substr(p)).c_str());
			py_value* v_value = eval_full(value, p, &np);
			if(np == string::npos) // error reading value!
				break;
			d->value[*name] = v_value;
			delete name;
			//printf("np is at %s\n", repr(value.substr(np)).c_str());			
			np = value.find_first_not_of(" \r\n\t", np + 1);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted , or } in %s", repr(value).c_str());
			if(value[np] == '}')
				break;
			if(value[np] != ',')
				throw str_exception_tb("wanted } or ' - got %c in %s", value[np], repr(value).c_str());
			p = np + 1;
			//printf("np now is at %s\n", repr(value.substr(p)).c_str());
		}
	} else if(value[p] == '[') {
		py_list* l = new py_list();
		output = l;
		p++;
		while(p < value.size()) { // todo: stop erlier!
			// read value
			//printf("try to read value from %s\n", repr(value.substr(p)).c_str());

			np = value.find_first_not_of(" \r\n\t", p);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted ] in %s", repr(value).c_str());
			if(value[np] == ']')
				break;

			py_value* v_value = eval_full(value, p, &np);
			// printf("got value %s and pos %d\n", repr(*v_value).c_str(), np);
			l->value.push_back(v_value);
 			//printf("np is at %s\n", repr(value.substr(np)).c_str());
			np = value.find_first_not_of(" \r\n\t", np + 1);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted , in %s", repr(value).c_str());
			if(value[np] == ']')
				break;
			if(value[np] != ',')
				throw str_exception_tb("expected , - got %c. in string %s", value[np], repr(value.substr(np - 3, 15)).c_str());
			p = np + 1;
			//printf("np now is at %s\n", repr(value.substr(np)).c_str());
		}
	} else if(value[p] == '(') {
		py_list* l = new py_tuple();
		output = l;
		p++;
		while(p < value.size()) { // todo: stop erlier!
			// read value
			//printf("try to read value from %s\n", repr(value.substr(p)).c_str());

			np = value.find_first_not_of(" \r\n\t", p);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted ) in %s", repr(value).c_str());
			if(value[np] == ')')
				break;

			py_value* v_value = eval_full(value, p, &np);
			// printf("got value %s and pos %d\n", repr(*v_value).c_str(), np);
			l->value.push_back(v_value);
 			//printf("np is at %s\n", repr(value.substr(np)).c_str());
			np = value.find_first_not_of(" \r\n\t", np + 1);
			if(np == string::npos)
				throw str_exception_tb("unexpected eos - wanted , in %s", repr(value).c_str());
			if(value[np] == ')')
				break;
			if(value[np] != ',')
				throw str_exception_tb("expected , - got %c. in string %s", value[np], repr(value.substr(np - 3, 15)).c_str());
			p = np + 1;
			//printf("np now is at %s\n", repr(value.substr(np)).c_str());
		}
	} else if(value[p] == '"' || value[p] == '\'') {
		py_string* s = new py_string();
		output = s;
		// read value
		//printf("try to read string from %s\n", repr(value.substr(p)).c_str());
		np = eval_string_until(value, p, s->value);
	} else if(((p + 1) < value.size()) && value[p] == '0' && (value[p + 1] == 'x' || value[p + 1] == 'X')) {
		string v;
		np = eval_string_until(value, p, v);
		py_int* s = new py_int();
		output = s;
		s->value = (int)strtol(v.c_str(), (char **)NULL, 16);
	} else if(isdigit((unsigned char)value[p]) || strchr("+-.", value[p])) {
		string v;
		np = eval_string_until(value, p, v);
		if(v.find(".") != string::npos || v.find("e") != string::npos || v.find("E") != string::npos) {
			py_float* s = new py_float();
			output = s;
			s->value = atof(v.c_str());
		} else if(v.substr(v.size() - 1, 1) == "L") {
			py_long* s = new py_long();
			output = s;
			s->value = atol(v.c_str());
		} else {
			py_int* s = new py_int();
			output = s;
			s->value = atoi(v.c_str());
		}
	} else if(value.substr(p, 4) == "True" || value.substr(p, 5) == "False"|| value.substr(p, 4) == "None") {
		py_special* s = new py_special();
		output = s;
		np = eval_string_until(value, p, s->value);
	} else {
		//throw str_exception_tb("unknown python value %s\n", repr(value.substr(p)).c_str());
		py_string* s = new py_string();
		output = s;
		eval_string_until(value, p, s->value);
	}
	if(np_out)
		*np_out = np;
	//printf("terminating with np at %s\n",
	//repr(value.substr(np)).c_str());
	/*
	printf("recognized token %s in ->%s<-\n", repr(output).c_str(),
			value.substr(start, np - start).c_str());
	*/
	return output;
}


string string_replace(string data, string search, string replace) {
	string::size_type p = 0;
	while(p < data.size()) {
		string::size_type np = data.find(search, p);
		if(np == string::npos)
			break;
		data.replace(np, search.size(), replace);
		p = np + replace.size();
	}
	return data;
}

string strip(string input, string white) {
	string::size_type start = input.find_first_not_of(white);
	if(start == string::npos)
		return "";
	string::size_type end = input.find_last_not_of(white);
	return input.substr(start, end - start + 1);
}
string lstrip(string input, string white) {
	string::size_type start = input.find_first_not_of(white);
	if(start == string::npos)
		return "";
	return input.substr(start, string::npos);
}
string rstrip(string input, string white) {
	string::size_type end = input.find_last_not_of(white);
	return input.substr(0, end + 1);
}

string format_string(const char* format, ...) {
	int n;
	int size = 0;
	char* p = NULL;
	char* np;

	if(!p) {
		size = 255;
		if ((p = (char*)malloc(size)) == NULL)
			throw std::bad_alloc();
	}
	va_list ap;
	// va_start(ap, format); // caused segfault with gcc 4.8.2 on x86_64 when vsnprintf needs to be called multiple times
	
	while(true) {
		/* Try to print in the allocated space. */
		va_start(ap, format); // needed for gcc 4.8.2 on x86_64
		n = vsnprintf(p, size, format, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			break;
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np = (char*)realloc(p, size)) == NULL) {
			free(p);
			throw std::bad_alloc();
		}
		p = np;
	}

	// va_end(ap);
	string ret(p);
	free(p);
	return ret;
}

string_formatter::string_formatter() {
	buffer = NULL;
	size = 0;
}
string_formatter::string_formatter(unsigned int preallocate) {
	size = preallocate;
	buffer = (char*)malloc(size);
	if(!buffer)
		throw std::bad_alloc();
}
string_formatter::~string_formatter() {
	free(buffer);
}

#if __cplusplus < 201103L
// before C++11
string string_formatter::operator()(const char* format, ...) {
	int n;
	char*& p = buffer;
	char* np;

	if(!p) {
		size = strlen(format);
		if(size < 256)
			size = 256;
		if ((p = (char*)malloc(size)) == NULL)
			throw std::bad_alloc();
	}
	va_list ap;
	// va_start(ap, format); // caused segfault with gcc 4.8.2 on x86_64 when vsnprintf needs to be called multiple times
	
	while(true) {
		/* Try to print in the allocated space. */
		va_start(ap, format); // needed for gcc 4.8.2 on x86_64
		n = vsnprintf(p, size, format, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			break;
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np = (char*)realloc(p, size)) == NULL)
			throw std::bad_alloc();
		p = np;
	}

	// va_end(ap);
	return string(p);
}
#else
// since C++11
string string_formatter::operator()(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	std::string s = vformat(format, ap);
	va_end(ap);
	return s;
}

string string_formatter::vformat(const char* format, va_list vlist) {
	va_list ap;
	va_list ap2;
	va_copy(ap, vlist);
	va_copy(ap2, ap);
	
	unsigned int needed = std::vsnprintf(nullptr, 0, format, ap) + 1; // c++11
	va_end(ap);
	
	int n;
	char*& p = buffer;
	char* np;

	if(!p) {
		size = needed;
		if ((p = (char*)malloc(size)) == NULL)
			throw std::bad_alloc();
	} else if(size < needed) {
		if ((np = (char*)realloc(p, needed)) == NULL)
			throw std::bad_alloc();
		p = np;
		size = needed;
	}
	
	std::vsnprintf(p, size, format, ap2);
	va_end(ap2);
	return string(p);
}
#endif


list<string> split_command_line(string cmd) {
	list<string> l;
	string::size_type p = 0;
	while(p != string::npos && p < cmd.size()) {
		string::size_type np;
		// skip eading spaces
		p = cmd.find_first_not_of(" ", p);
		if(p == string::npos)
			break;
		// test first char of this argument!
		if(cmd[p] != '"' && cmd[p] != '\'') {
			np = cmd.find(' ', p);
			l.push_back(cmd.substr(p, np - p));
			p = np;
			continue;
		}
		char delim = cmd[p];
		string::size_type end_delim_start = p + 1;

		// search end of string!
		while(true) {
			np = cmd.find(delim, end_delim_start);
			if(np == string::npos)
				throw str_exception_tb("invalid command line! quoted argument %d is not terminated by a quote!", l.size() + 1);
			string::size_type termpos = np - 1;
			unsigned int backslash_count = 0;
			// TODO: backslash handling not finished! need to unescape!!
			while(cmd[termpos] == '\\') {
				backslash_count ++;
				termpos --;
			}
			if((backslash_count % 2) == 0)
				break;
			end_delim_start = np + 1;
			if(end_delim_start >= cmd.size())
				throw str_exception_tb("invalid command line! quoted argument %d is not terminated by a quote!", l.size() + 1);
		}
		l.push_back(cmd.substr(p + 1, np - p - 1));
		p = np + 1;
	}
	return l;
}

string join_command_line(list<string>& args) {
	stringstream ss;
	for(list<string>::iterator i = args.begin(); i != args.end(); i++) {
		if(i != args.begin())
			ss << " ";
		ss << "\"" << *i << "\"";
	}
	return ss.str();
}

vector<string> split_string(string input, string by, unsigned int max_split) {
	vector<string> out;

	string::size_type s = 0;
	while(s < input.size()) {
		string::size_type p = input.find(by, s);
		if(p == string::npos) {
			out.push_back(input.substr(s));
			break;
		}
		out.push_back(input.substr(s, p - s));
		s = p + by.size();

		if(max_split > 0 && out.size() == max_split) {
			out.push_back(input.substr(s));
			break;
		}
	}
	return out;
}
string join_string(vector<string> input, string by) {
	stringstream out;
	for(unsigned int i = 0; i < input.size(); i++) {
		if(i > 0)
			out << by;
		out << input[i];
	}
	return out.str();
}
string join_string(vector<string> input, string by, unsigned int from, unsigned int to_without) {
	stringstream out;
	for(unsigned int i = from; i < to_without; i++) {
		if(i > from)
			out << by;
		out << input[i];
	}
	return out.str();
}


bool pattern_matches(string pattern, string test) {
	/*
	  special chars: * ?

	  xyz*
	  *xyz
	*/
	
	// printf("\n\npattern: %s, test: %s\n", repr(pattern).c_str(), repr(test).c_str());
	if(pattern == "*") {
		// printf("pattern accepts all\n");
		return true;
	}
		
	string::size_type p_pos = 0;
	string::size_type t_pos = 0;

	string::size_type ungreedy = string::npos;

	char p = 0;
	char t = 0;
	while(!(p_pos == pattern.size() && t_pos == test.size())) {
		if(p_pos == pattern.size()) {
			// printf("pattern is at end, while text is at %d\n", t_pos);
			return false;
		}

		p = pattern[p_pos];
		// printf("pattern char: %c\n", p);
		if(p_pos == (pattern.size() - 1) && p == '*') {
			// printf("pattern has trailing *, match!\n");
			return true;
		}
		if(t_pos == test.size()) {
			if(p == '*') {
				if(ungreedy == string::npos) {
					// printf("pattern has * but no ungreedy match!\n");
					return false;
				}
				// printf("pattern has * and we have angreedy match at text %d\n", ungreedy);
				p_pos += 2;
				t_pos = ungreedy + 1;
				ungreedy = string::npos;
				continue;
			}
			// printf("text is at end, while pattern is at %d\n", p_pos);
			return false;
		}

		t = test[t_pos];
		// printf("text char: %c\n", t);

		if(p == '*') {
			char np = pattern[p_pos + 1];
			// printf("next pattern char: %c\n", np);
			if(np == t) {
				// printf("ungreedy will match at text %c\n", t);
				ungreedy = t_pos;
				p_pos += 2; // to allow patterns like "*.cd" for "ab.cd"
				t_pos ++;
				continue;
			}
			// maybe there is (another) later match!
			t_pos++;
			// printf("next text pos!\n");
			continue;
		} else if(p == '?') {
			p_pos ++;
			t_pos ++;
			continue;
		} else if(p == t) {
			p_pos ++;
			t_pos ++;
			continue;
		}
		// printf("test char %c does not match pattern char %c\n", t, p);
		return false;
	}
	// printf("both pos at end! match!\n");
	return true;
}

}
