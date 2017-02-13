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
#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdint.h>

#include "exceptions.h"

namespace string_util {

#define str_from_define(s) #s

typedef std::map<std::string, std::string> dict;

class py_value;

std::string binary_data_repr(const char* input, int input_len);
std::string binary_data_repr(const std::string& input);

std::string repr(const char* input, int input_len, bool use_dbl_quote=false);
std::string repr(const char* input, bool use_dbl_quote=false);
std::string repr(std::string input, bool use_dbl_quote=false);
std::string repr(py_value* input, bool use_dbl_quote=false);
std::string repr(py_value& input);

std::string repr(double input);
std::string repr(float input);
std::string repr(uint32_t input);
std::string repr(int32_t input);
std::string repr(uint64_t input);
std::string repr(int64_t input);
std::string repr(bool input);

class py_value {
public:
	virtual ~py_value() { };

	virtual operator std::string() const { return "<py_value>"; };
	virtual std::string repr() { return string_util::repr(std::string(*this)); };

	virtual operator bool() { return false; }
	virtual operator int() { return 0; }
	virtual operator unsigned int() { return 0; }
	virtual operator float() { return 0.0; }
	virtual operator double() { return 0.0; }

	virtual std::string type_name() { return "py_value"; }
	virtual py_value* copy() { return new py_value(); }
};

class py_string : public py_value {  
public:
	std::string value;

	py_string() {};
	py_string(std::string v) : value(v) {};
	py_string(char* v, int len) { value = std::string(v, len); };
	py_string(const py_string& a) { value = a.value; }
	virtual ~py_string() {};

	virtual operator bool() { return bool(value.size()); }
	virtual operator std::string() const { return value; }
	virtual operator int() { return atoi(value.c_str()); };
	virtual std::string type_name() { return "py_string"; }

	virtual py_value* copy() { return new py_string(value); }
};

class py_int : public py_value {
public:
	int value;

	py_int() {};
	py_int(int v) : value(v) {};
	virtual ~py_int() {};

	virtual operator std::string() const {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual operator bool() { return bool(value); }
	virtual operator int() { return value; };
	virtual operator float() { return float(value); }
	virtual operator double() { return double(value); }
	virtual operator unsigned int() { return value; }
	virtual std::string type_name() { return "py_int"; }

	virtual py_value* copy() { return new py_int(value); }
};

typedef int64_t py_long_value_t;
class py_long : public py_value {
public:
	py_long_value_t value;

	py_long() {};
	py_long(py_long_value_t v) : value(v) {};
	virtual ~py_long() {};

	virtual operator std::string() const {
		std::stringstream ss;
		ss << value << "L";
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual operator bool() { return bool(value); }
	virtual operator int() { return (int)value; };
	virtual operator py_long_value_t() { return value; };
	virtual operator float() { return float(value); }
	virtual operator double() { return double(value); }
	virtual operator unsigned int() { return (unsigned int)value; }
	virtual std::string type_name() { return "py_long"; }

	virtual py_value* copy() { return new py_long(value); }
};

class py_float : public py_value {
public:
	double value;

	py_float() {};
	py_float(double v) : value(v) {};
	virtual ~py_float() {};

	virtual operator std::string() const {
		std::stringstream ss;
		ss << std::setprecision(15) << value;
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual operator float() { return (float)value; };
	virtual operator double() { return value; };
	virtual std::string type_name() { return "py_float"; }

	virtual py_value* copy() { return new py_float(value); }
};

class py_special : public py_value {
public:
	std::string value;

	py_special() : value("None") {};
	py_special(std::string v) : value(v) {};
	virtual ~py_special() {};

	virtual operator bool() { return value == "True" || value == "true"; }
	virtual operator std::string() const { return value; }
	virtual std::string repr() { return std::string(*this); };
	virtual std::string type_name() { return "py_special"; }

	virtual py_value* copy() { return new py_special(value); }
};

typedef std::list<py_value*> py_list_value_t;
class py_list : public py_value {  
public:
	py_list_value_t value;

	virtual ~py_list() {
		for(py_list_value_t::iterator i = value.begin(); i != value.end(); i++)
			delete *i;
	}

	virtual operator std::string() const {
		std::stringstream ss;
		ss << "[";
		for(py_list_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
			if(i != value.begin())
				ss << ", ";
			ss << string_util::repr(*i);
		}
		ss << "]";
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual std::string type_name() { return "py_list"; }

	virtual py_value* copy() { 
		py_list* pyl = new py_list(); 
		for(py_list_value_t::iterator i = value.begin(); i != value.end(); i++)
			pyl->value.push_back((*i)->copy());
		return pyl;
	}
};


class py_tuple : public py_list {  
public:

	virtual operator std::string() const {
		std::stringstream ss;
		ss << "(";
		for(py_list_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
			if(i != value.begin())
				ss << ", ";
			ss << string_util::repr(*i);
		}
		ss << ")";
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual std::string type_name() { return "py_tuple"; }
};


typedef std::map<std::string, py_value*> py_dict_value_t;
class py_dict : public py_value {  
public:
	py_dict_value_t value;

	py_dict() {};
	py_dict(const py_dict& v) : value(v.value) {};
	virtual ~py_dict() {
		for(py_dict_value_t::iterator i = value.begin(); i != value.end(); i++)
			delete i->second;
	}

	py_value*& operator [](const std::string& v) {
		return value[v];
	}
	virtual operator std::string() const {
		std::stringstream ss;
		ss << "{";
		for(py_dict_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
			if(i != value.begin())
				ss << ", ";
			ss << string_util::repr(i->first) << ":" << string_util::repr(i->second);
		}
		ss << "}";
		return ss.str();
	}
	virtual std::string repr() { return std::string(*this); };
	virtual std::string type_name() { return "py_dict"; }

	virtual py_value* copy() { 
		py_dict* pyd = new py_dict(); 
		for(py_dict_value_t::iterator i = value.begin(); i != value.end(); i++)
			pyd->value[i->first] = i->second->copy();
		return pyd;
	}
};

std::string eval(std::string value);
std::string::size_type eval_string_until(std::string value, std::string::size_type start, std::string& output);
dict eval_dict(std::string value);
std::list<std::string> eval_list(std::string value);
py_value* eval_full(std::string value, std::string::size_type start=0, std::string::size_type* np_out=NULL);

unsigned int hex2dec(std::string hex);

std::string string_replace(std::string data, std::string search, std::string replace);
std::string strip(std::string input, std::string white="\r\n\t ");
std::string rstrip(std::string input, std::string white="\r\n\t ");
std::string lstrip(std::string input, std::string white="\r\n\t ");
std::string format_string(const char* format, ...);

std::list<std::string> split_command_line(std::string cmdline);
std::string join_command_line(std::list<std::string>& args);

std::vector<std::string> split_string(std::string input, std::string by, unsigned int max_split=0);
std::string join_string(std::vector<std::string> input, std::string by);
std::string join_string(std::vector<std::string> input, std::string by, unsigned int from, unsigned int to_without);

bool pattern_matches(std::string pattern, std::string test);

}
#endif // STRING_UTIL_H
