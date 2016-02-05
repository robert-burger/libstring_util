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

using namespace std;

#define str_from_define(s) #s

typedef map<string, string> dict;

class py_value;

string binary_data_repr(const char* input, int input_len);
string binary_data_repr(const string& input);

string repr(const char* input, int input_len, bool use_dbl_quote=false);
string repr(const char* input, bool use_dbl_quote=false);
string repr(string input, bool use_dbl_quote=false);
string repr(py_value* input, bool use_dbl_quote=false);
string repr(py_value& input);

string repr(double input);
string repr(float input);
string repr(uint32_t input);
string repr(int32_t input);
string repr(uint64_t input);
string repr(int64_t input);
string repr(bool input);

class py_value {
 public:
  virtual ~py_value() { };

  virtual operator string() const { return "<py_value>"; };
  virtual string repr() { return ::repr(string(*this)); };

  virtual operator bool() { return false; }
  virtual operator int() { return 0; }
  virtual operator unsigned int() { return 0; }
  virtual operator float() { return 0.0; }
  virtual operator double() { return 0.0; }

  virtual string type_name() { return "py_value"; }
  virtual py_value* copy() { return new py_value(); }
};

class py_string : public py_value {  
 public:
  string value;

  py_string() {};
  py_string(string v) : value(v) {};
  py_string(char* v, int len) { value = string(v, len); };
  py_string(const py_string& a) { value = a.value; }
  virtual ~py_string() {};

  virtual operator bool() { return bool(value.size()); }
  virtual operator string() const { return value; }
  virtual operator int() { return atoi(value.c_str()); };
  virtual string type_name() { return "py_string"; }

  virtual py_value* copy() { return new py_string(value); }
};

class py_int : public py_value {
 public:
	int value;

  py_int() {};
  py_int(int v) : value(v) {};
  virtual ~py_int() {};

  virtual operator string() const {
    stringstream ss;
    ss << value;
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual operator bool() { return bool(value); }
  virtual operator int() { return value; };
  virtual operator float() { return float(value); }
  virtual operator double() { return double(value); }
  virtual operator unsigned int() { return value; }
  virtual string type_name() { return "py_int"; }

  virtual py_value* copy() { return new py_int(value); }
};

typedef int64_t py_long_value_t;
class py_long : public py_value {
public:
	py_long_value_t value;

  py_long() {};
  py_long(py_long_value_t v) : value(v) {};
  virtual ~py_long() {};

  virtual operator string() const {
    stringstream ss;
    ss << value << "L";
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual operator bool() { return bool(value); }
  virtual operator int() { return (int)value; };
  virtual operator py_long_value_t() { return value; };
  virtual operator float() { return float(value); }
  virtual operator double() { return double(value); }
  virtual operator unsigned int() { return (unsigned int)value; }
  virtual string type_name() { return "py_long"; }

  virtual py_value* copy() { return new py_long(value); }
};

class py_float : public py_value {
 public:
  double value;

  py_float() {};
  py_float(double v) : value(v) {};
  virtual ~py_float() {};

  virtual operator string() const {
    stringstream ss;
    ss << std::setprecision(15) << value;
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual operator float() { return value; };
  virtual operator double() { return value; };
  virtual string type_name() { return "py_float"; }

  virtual py_value* copy() { return new py_float(value); }
};

class py_special : public py_value {
 public:
  string value;

  py_special() : value("None") {};
  py_special(string v) : value(v) {};
  virtual ~py_special() {};

  virtual operator bool() { return value == "True" || value == "true"; }
  virtual operator string() const { return value; }
  virtual string repr() { return string(*this); };
  virtual string type_name() { return "py_special"; }

  virtual py_value* copy() { return new py_special(value); }
};

typedef list<py_value*> py_list_value_t;
class py_list : public py_value {  
 public:
  py_list_value_t value;

  virtual ~py_list() {
    for(py_list_value_t::iterator i = value.begin(); i != value.end(); i++)
      delete *i;
  }

  virtual operator string() const {
    stringstream ss;
    ss << "[";
    for(py_list_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
      if(i != value.begin())
        ss << ", ";
      ss << ::repr(*i);
    }
    ss << "]";
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual string type_name() { return "py_list"; }

  virtual py_value* copy() { 
	  py_list* pyl = new py_list(); 
	  for(py_list_value_t::iterator i = value.begin(); i != value.end(); i++)
		  pyl->value.push_back((*i)->copy());
	  return pyl;
  }
};


class py_tuple : public py_list {  
 public:

  virtual operator string() const {
    stringstream ss;
    ss << "(";
    for(py_list_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
      if(i != value.begin())
        ss << ", ";
      ss << ::repr(*i);
    }
    ss << ")";
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual string type_name() { return "py_tuple"; }
};


typedef map<string, py_value*> py_dict_value_t;
class py_dict : public py_value {  
 public:
  py_dict_value_t value;

  py_dict() {};
  py_dict(const py_dict& v) : value(v.value) {};
  virtual ~py_dict() {
    for(py_dict_value_t::iterator i = value.begin(); i != value.end(); i++)
      delete i->second;
  }

  py_value*& operator [](const string& v) {
    return value[v];
  }
  virtual operator string() const {
    stringstream ss;
    ss << "{";
    for(py_dict_value_t::const_iterator i = value.begin(); i != value.end(); i++) {
      if(i != value.begin())
        ss << ", ";
      ss << ::repr(i->first) << ":" << ::repr(i->second);
    }
    ss << "}";
    return ss.str();
  }
  virtual string repr() { return string(*this); };
  virtual string type_name() { return "py_dict"; }

  virtual py_value* copy() { 
	  py_dict* pyd = new py_dict(); 
	  for(py_dict_value_t::iterator i = value.begin(); i != value.end(); i++)
		  pyd->value[i->first] = i->second->copy();
	  return pyd;
  }
};

string eval(string value);
string::size_type eval_string_until(string value, string::size_type start, string& output);
dict eval_dict(string value);
list<string> eval_list(string value);
py_value* eval_full(string value, string::size_type start=0, string::size_type* np_out=NULL);

unsigned int hex2dec(string hex);

string string_replace(string data, string search, string replace);
string strip(string input, string white="\r\n\t ");
string rstrip(string input, string white="\r\n\t ");
string lstrip(string input, string white="\r\n\t ");
string format_string(const char* format, ...);

list<string> split_command_line(string cmdline);
string join_command_line(list<string>& args);

vector<string> split_string(string input, string by, unsigned int max_split=0);
string join_string(vector<string> input, string by);
string join_string(vector<string> input, string by, unsigned int from, unsigned int to_without);

bool pattern_matches(string pattern, string test);


#endif // STRING_UTIL_H
