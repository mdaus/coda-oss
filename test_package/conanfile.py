from conans import ConanFile, CMake
import os

class CodaOssTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = ("cmake", "cmake_paths")

    def build(self):
        cmake = CMake(self)
        for name, val in self.options['coda-oss'].items():
            if name.isupper() and val is not None:
                cmake.definitions[name] = val
        cmake.configure()
        cmake.build()

    def test(self):
        self.run(os.path.join("bin", "test"), run_environment=True)
