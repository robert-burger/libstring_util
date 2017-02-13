check_PROGRAMS = example

example_SOURCES = example.cpp
example_CXXFLAGS = -I$(top_srcdir)/include
example_LDADD = -L$(top_builddir)/src/ -lstring_util

noinst_DATA = expected_output
TESTS = example check_output.test

check_output.log: example.log
