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

        if self.settings.build_type == "Debug":
            autotools.flags = ["-O0", "-g"]
        else:
            autotools.flags = ["-O3"]

        if self.options.shared:
            args = ["--enable-shared", "--disable-static"]
        else:
            args = ["--disable-shared", "--enable-static"]

        autotools.configure(configure_dir=".", vars={'CFLAGS': ''}, args=args)
        autotools.make()
        autotools.make(target="check", args="VERBOSE=1")

    def package(self):
        autotools = AutoToolsBuildEnvironment(self)
        autotools.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        self.cpp_info.libdirs = ['lib', 'lib64']
        self.cpp_info.libs = ["string_util"]
