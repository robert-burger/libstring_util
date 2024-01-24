import os

from conan import ConanFile
from conan.tools.files import replace_in_file, rm
from conan.tools.gnu import Autotools


class StringUtilConan(ConanFile):
    name = "libstring_util"
    license = "GPLv3"
    url = f"https://rmc-github.robotic.dlr.de/common/{name}"
    author = "Florian Schmidt <florian.schmidt@dlr.de>"
    description = "simple string operations"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = ["*", "!.gitignore"]
    generators = "AutotoolsToolchain"

    def build(self):
        replace_in_file(
            self, "project.properties", "VERSION=1.1.7", f"VERSION={self.version.replace('.', ':').replace('-', '')}"
        )
        self.run("autoreconf -if")
        autotools = Autotools(self)
        autotools.fpic = self.options.fPIC

        if self.options.shared:
            args = ["--enable-shared", "--disable-static"]
        else:
            args = ["--disable-shared", "--enable-static"]

        autotools.configure(args=args)
        autotools.make(args=["V=1"])

        if os.getenv("BUILD_MODE") == "cross" or os.getenv("CHOST"):
            # we are cross compiling,
            # just test whether test-program compiles
            not_working = (
                "x86_64-w64-mingw32",
                "i686-w64-mingw32",
            )  # those are hosts where somehow autotools fails to provide include path as requested in Makefile.am
            if os.getenv("CHOST") not in not_working:
                autotools.make(target="example", args=["-C", "test/example", "V=1"])
        else:
            autotools.make(target="check", args=["VERBOSE=1"])

    def package(self):
        autotools = Autotools(self)
        autotools.install()
        for file in os.listdir(self.package_folder):
            if file.endswith(".la"):
                rm(self, os.path.join(self.package_folder, file))

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib", "lib64"]
        self.cpp_info.libs = ["string_util"]
