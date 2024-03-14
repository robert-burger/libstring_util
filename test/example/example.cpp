#include <stdio.h>
#include <assert.h>

#include <string_util/string_util.h>

using namespace string_util;

/*
  expected output:
  N list items: 3
  int: 123
  float: 123.456000
  string: 'string'
  python repr: [123, 123.456, 'string']
 */

int main(int argc, char* argv[]) {	
	py_value* v = eval_full("[123, 123.456, 'string']"); // v is now the owner!

	py_list* l = dynamic_cast<py_list*>(v); assert(l);	
	printf("N list items: %ld\n", l->value.size());

	for(py_list_value_t::iterator i = l->value.begin(); i != l->value.end(); i++) {
		py_int* iv = dynamic_cast<py_int*>(*i);
		py_float* fv = dynamic_cast<py_float*>(*i);
		py_string* sv = dynamic_cast<py_string*>(*i);

		if(iv)
			printf("int: %d\n", iv->value);
		else if(fv)
			printf("float: %f\n", fv->value);
		else if(sv)
			printf("string: '%s'\n", sv->value.c_str());
		else
			printf("other object: %s\n", repr(*i).c_str());
	}

	l->value.push_back(new py_string("echo 'done, you can now poweroff...'"));
	l->value.push_back(new py_long(4242)); // takes ownership of new py_long
 	printf("python repr: %s\n", repr(v).c_str());

	show_py2_L_literal = true;
	printf("python repr of py_long with L-literal: %s\n", repr(l->value.back()).c_str());


	delete v;
	return 0;
}
