import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy

class DownloadManager(ConanFile):
    name = "downloadManagerV2"
    version = "0.1.0"

    settings = "os", "arch", "compiler", "build_type"
    package_type = "application"

    requires = (
        "sdl/3.4.8",
        "libcurl/8.20.0",
        "spdlog/1.17.0",
        "imgui/1.92.8"
    )
    def configure(self):
        self.options["sdl"].shared = "True"
        self.options["imgui"].with_sdl3_binding = "True"
        if self.settings.os == "Windows":
            self.options["sdl"].shared = "False"
            self.options["openssl"].no_apps = "True"
            self.options["openssl"].no_dgram = "True"
    def layout(self):
        cmake_layout(self)

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.generate()
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"), os.path.join(self.source_folder, "bindings"))

        dependencies = CMakeDeps(self)
        dependencies.generate()
