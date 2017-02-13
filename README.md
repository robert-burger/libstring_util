# contents

* simple string operations:
 * string_replace(heystack, needle, repl) -> string
 * [{r,l}]strip(input, white="\r\n\t ") -> string
 * format_string(printf_format, args...) -> string
 * split_string(input, by, max=0) -> vector<string>
 * join_string(vector<string> input, string by) -> string

* (de-)serialization to python source
```C++
    py_value* v = eval_full("[123, 123.456, 'string']");
    printf("python repr: %s\n", repr(v).c_str());
    // output: python repr: [123, 123.456, 'string']
```
check `test/example/example.cpp`
   
# building

libstring_util has at least these build requirements:

 * g++
 * make
 
on debian (tested on jessie):

    $ apt-get install build-essential

## generate build system
if there is no file named "configure" you will need to generate the
autotools-based build-system first. for that you will need these
additional requirements:

 * autoconf (e.g. 2.69-8)
 * automake (e.g. 1.14)
 * libtool (e.g. 2.4.2-1.11)

on debian:

    $ apt-get install automake libtool

to create the buildsystem go into this directory and execute:

    $ autoreconf -if

this will generate a configure-file.

## configure build system
you should build out of source tree. for that create a new directory
and execute configure with your desired flags (try `--help` to see a
list of possible options):

    $ mkdir build
    $ cd build
    $ ../configure --prefix=/usr/local

## start build
after configure successfully finished, you can call make to build the
libstring_util library:

    $ make -j4
	
on success the generated libraries can be found at `build/src/.libs/`
	
## install
if you want to install the generated libraries to your system (to the
above specified `--prefix`) you can use the `install` target:

    $ make install

depending on where you want to install, you might need root-rights:

    $ sudo make install


