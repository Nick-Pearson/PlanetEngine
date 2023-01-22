from conans import ConanFile, CMake

class PlanetEngine(ConanFile):
   settings = "os", "compiler", "build_type", "arch"
   requires = [
      "spdlog/1.8.2",
      "stb/20200203",
      "imgui/1.88",
      "libpng/1.6.37",
      "openfbx/cci.20210426",
      "wxwidgets/3.1.5@bincrafters/stable",
      "zlib/1.2.11"
   ]
   build_requires = "gtest/1.10.0"
   generators = "cmake", "gcc", "txt"

   def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()

   def configure(self):
      self.options["wxwidgets"].tiff = "off"
