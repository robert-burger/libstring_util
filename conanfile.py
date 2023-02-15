import os
from conans import ConanFile, AutoToolsBuildEnvironment, tools


class StringUtilConan(ConanFile):
    name = "libstring_util"
    license = "GPLv3"
    url = f"https://rmc-github.robotic.dlr.de/common/{name}"
    author = "Florian Schmidt <florian.schmidt@dlr.de>"
    description = "simple string operations"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = ["*", "!.gitignore"] + ["!%s" % x for x in tools.Git().excluded_files()]

    def build(self):
        self.run("autoreconf -if")
        autotools = AutoToolsBuildEnvironment(self)
        autotools.fpic = self.options.fPIC

        if self.options.shared:
            args = ["--enable-shared", "--disable-static"]
        else:
            args = ["--disable-shared", "--enable-static"]

        autotools.configure(configure_dir=".", args=args)
        autotools.make(args=["V=1"])

        if os.getenv("BUILD_MODE") == "cross" or os.getenv("CHOST"):
            # we are cross compiling,
            # just test whether test-program compiles
            not_working = ("x86_64-w64-mingw32", "i686-w64-mingw32") # those are hosts where somehow autotools fails to provide include path as requested in Makefile.am
            if os.getenv("CHOST") not in not_working:
                autotools.make(target="example", args=["-C", "test/example", "V=1"])
        else:
            autotools.make(target="check", args=["VERBOSE=1"])
        
    def package(self):
        autotools = AutoToolsBuildEnvironment(self)
        autotools.install()
        tools.remove_files_by_mask(self.package_folder, "*.la")

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libdirs = ['lib', 'lib64']
        self.cpp_info.libs = ["string_util"]
