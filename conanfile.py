from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout

class ZkllvmConanfile(ConanFile):
    name = "zkllvm"
    version = "1.0"
    description = "A basic recipe"
    license = "<Your project license goes here>"
    homepage = "<Your project homepage goes here>"
    exports_sources = "bin/*", "build/*", "cmake/*", "CMakeLists.txt", "libs/*", "examples/*", ".git/*", "detect-platform.py"
    no_copy_source = True 
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_tests": [True, False],
        "with_rslang": [True, False],
        "rslang_build_tools": [None, "ANY"],
        "with_evm_verifier": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "with_tests": False,
        "with_rslang": False,
        "rslang_build_tools": "cargo",
        "with_evm_verifier": True
    }

    #def source(self):
    #    download(self, f"hhttps://github.com/NilFoundation/zkLLVM/archive/refs/tags/{self.version}.zip", zip_name)
    #    # Recommended practice, always check hashes of downloaded files
    #    check_sha1(self, zip_name, "8d87812ce591ced8ce3a022beec1df1c8b2fac87")
    #    unzip(self, zip_name)
    #    shutil.move(f"zkllvm-{self.version}", "zkllvm")
    #    os.unlink(zip_name)
    
    def validate(self):
        if self.settings.os == "Windows":
            raise ConanInvalidConfiguration(f"compilation for Windows is not supported")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
       cmake_layout(self, build_folder="build")

    # The requirements method allows you to define the dependencies of your recipe
    def requirements(self):
        # Each call to self.requires() will add the corresponding requirement
        # to the current list of requirements
        self.requires("boost/1.84.0")

    # The build_requirements() method is functionally equivalent to the requirements() one,
    # being executed just after it. It's a good place to define tool requirements,
    # dependencies necessary at build time, not at application runtime
    def build_requirements(self):
        # Each call to self.tool_requires() will add the corresponding build requirement
        # Uncommenting this line will add the cmake >=3.15 build dependency to your project
        self.tool_requires("cmake/[>=3.29]")

    # The purpose of generate() is to prepare the build, generating the necessary files, such as
    # Files containing information to locate the dependencies, environment activation scripts,
    # and specific build system files among others
    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["BUILD_WITH_BOOST_STATIC_LIBS"] = not self.dependencies["boost"].options.shared
        tc.variables["BUILD_TESTS"] = self.options.with_tests
        tc.variables["RSLANG_BUILD_EXTENDED"] = self.options.with_rslang
        tc.variables["RSLANG_BUILD_TOOLS"] = self.options.rslang_build_tools
        tc.variables["GENERATE_EVM_VERIFIER"] = self.options.with_evm_verifier
        tc.generate()

        tc = CMakeDeps(self)
        tc.generate()

    # This method is used to build the source code of the recipe using the desired commands.
    def build(self):
        # You can use your command line tools to invoke your build system
        # or any of the build helpers provided with Conan in conan.tools
        # self.run("g++ ...")
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    # The actual creation of the package, once it's built, is done in the package() method.
    # Using the copy() method from tools.files, artifacts are copied
    # from the build folder to the package folder
    def package(self):
        #cmake = CMake(self)
        #cmake.install()
        # copy(self, "*.h", self.source_folder, join(self.package_folder, "include"), keep_path=False)
        pass
