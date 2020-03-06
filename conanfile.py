from conans import ConanFile, CMake

class CollectionConan(ConanFile):
    name = "collection"
    version = "latest"
    license = "Proprietary"
    url = "https://github.com/Gundoll/collection"
    description = "Collection of C Data Structures"

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

    def package(self):
        self.copy("*.dll", dst="lib", keep_path=False)
        self.copy("*.h", dst="include/collection", src="collection", excludes="build*")
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["collection"]
