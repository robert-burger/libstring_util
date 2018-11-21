from conans import ConanFile, AutoToolsBuildEnvironment


class StringUtilConan(ConanFile):
    name = "libstring_util"
    license = "GPLv3"
    url = f"https://rmc-github.robotic.dlr.de/common/{name}}"
    description = "simple string operations"
    settings = "os", "compiler", "build_type", "arch"
    options = {"fPIC": [True, False]}
    default_options = "fPIC=True"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive",
    }

    def build(self):
        self.run("autoreconf -if")
        autotools = AutoToolsBuildEnvironment(self)
        autotools.fpic = self.options.fPIC
        autotools.configure(configure_dir=".", vars={'CFLAGS': ''})
        autotools.make()

    def package(self):
        autotools = AutoToolsBuildEnvironment(self)
        autotools.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libdirs = ['lib', 'lib64']
        self.cpp_info.libs = ["string_util"]
