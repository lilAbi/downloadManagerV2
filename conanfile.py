from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps

class DownloadManager(ConanFile):
    name = "downloadManagerV2"
    version = "0.1.0"

    settings = "os", "arch", "compiler", "build_type"
    package_type = "application"

    requires = (
        "sdl/3.4.8",
        "libcurl/8.20.0",
        "spdlog/1.17.0"
    )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.generate()

        dependencies = CMakeDeps(self)
        dependencies.generate()
