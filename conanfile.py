from conans import ConanFile, CMake, tools

class DDSConan(ConanFile):
    name = "collection"
    version = "latest"
    license = "MIT"
    url = "https://github.com/Gundoll/collection.git"
    description = "Data structure & algorithm collections written in C"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/Gundoll/collection.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="collection")
        cmake.build()
