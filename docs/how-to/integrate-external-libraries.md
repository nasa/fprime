# How-To: Integrate a Third-Party Library

> [!IMPORTANT]
> For this How-To guide and many other advanced F´ CMake use cases, it is recommended to have a basic understanding of CMake. If you are unfamiliar with CMake, please refer to the [CMake documentation](https://cmake.org/getting-started/). They provide a [great tutorial to get you started](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).

It is common practice to use third-party libraries in F´ projects. CMake provides multiple ways to integrate external libraries into a project, and the F´ build system facilitates it even further. This document will cover four different approaches to integrate external libraries into your F´ project design and build process. You should also refer to each library's documentation for any specific requirements or recommendations for integration, as there are many different ways to build and use libraries in CMake.

> [!NOTE]
> The source code for the examples shown in this guide can be found in our examples repo here: [https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs)

## Common considerations

CMake provides a [Using Dependencies Guide](https://cmake.org/cmake/help/v3.31/guide/using-dependencies/index.html) which we recommend reading for a deeper understanding of how to integrate dependencies into your CMake projects.

When integrating a library into your F´ project, there are considerations based on how the library is built and what it provides.

- **Source or Pre-compiled**: Do you have the source code for the library, or is it pre-compiled (installed on your system, or `.a / .so` files available)?
- **Build System** _(only applies if you have the source)_: Does the library use CMake, or another build system?
- **CMake integration**: Does the library provide CMake configuration files such as `<libName>Config.cmake`? (refer to the [CMake documentation](https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html) for more information)

The following summarizes recommendation on which approach to use based on these considerations:

1. **Pre-compiled**:
  a) If you only have a pre-compiled library and no CMake integration, use `MOD_DEPS` (Approach 1).
  b) If you have a pre-compiled library along with CMake integration, use `find_package()` (Approach 3).
2. **Source**:
  a) If the library uses CMake, use `FetchContent / add_subdirectory` (Approach 2).
  b) If the library does not use CMake but provides CMake integration, use `find_package()` (Approach 3) or `ExternalProject_Add()` (Approach 4).
  c) If the library does not use CMake and provides no CMake integration, use `ExternalProject_Add()` (Approach 4).

Note that these are mere recommendations, and that you may choose to use a different approach based on your project's needs or the library's requirements. The following sections will cover each of these approaches in detail, along with their benefits and drawbacks.

---

## Approach 1: Pre-compiled library files

|     |     |
| --- | --- |
| **Benefits** | Easy to set up; no additional build time |
| **Drawbacks** | Not portable across platforms |
| **Considerations** | May be provided by a vendor or third-party |

A pre-compiled library is a library that has already been compiled and is ready to be used, often named `lib<libName>.a` or `lib<libName>.so`. There are many ways to obtain pre-compiled libraries, such as downloading them from a vendor repository or building them from source yourself.

To integrate a pre-compiled library, you need to add the path of that library file to `MOD_DEPS` of the module(s) that depend on it (MOD_DEPS: module dependencies). The following example demonstrates how to integrate the OpenSSL `libcrypto` library into an F´ wrapper component:

```cmake
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/OpenSslWrapper.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/OpenSslWrapper.cpp"
)
set(MOD_DEPS
    ${FPRIME_PROJECT_ROOT}/lib/openssl/libcrypto.a
)
register_fprime_module()
target_include_directories(${FPRIME_CURRENT_MODULE} PUBLIC "${FPRIME_PROJECT_ROOT}/lib/openssl/include")
```

This assumes that the `libcrypto.a` is available in the `lib/openssl/` directory of your F´ project root, and that the necessary header files are in `lib/openssl/include/`.

---

## Approach 2: CMake library with FetchContent or add_subdirectory

|     |     |
| --- | --- |
| **Benefits** | Very easy to set up; portable |
| **Drawbacks** | Not guaranteed to handle transitive dependencies |
| **Considerations** | Recommended approach if available |

Many popular libraries use CMake as their build system and should likely be available through the [FetchContent module](https://cmake.org/cmake/help/v3.31/module/FetchContent.html) or a `add_subdirectory()` call, allowing for very simple integration. 

FetchContent will download and add the library to your build, often through a `add_subdirectory()` call ([reference](https://cmake.org/cmake/help/latest/module/FetchContent.html#command:fetchcontent_makeavailable)). If the library is already in your project's source, for example with a git submodule, you can use `add_subdirectory()` directly.

> [!NOTE]
> Some libraries may not build properly with this method. OpenCV, for example, [does not support it](https://github.com/opencv/opencv/issues/26955#issuecomment-2690702771) due to the complexity of its CMake set up (as of April 2025). In such cases, you will need to use `find_package()` or `ExternalProject_Add` as described in the sections below.


##### Step 1: Use add_subdirectory (or equivalent FetchContent) to include the library in your project

The following excerpt demonstrates how to integrate the [ETL libraries](https://github.com/ETLCPP/etl) using `FetchContent`:

```cmake
# This code can be located in the root project.cmake file if the library is used by multiple modules
# or directly in the module's CMakeLists.txt if it is only used by that module.
include(FetchContent)
FetchContent_Declare(
    etl
    GIT_REPOSITORY https://github.com/ETLCPP/etl
    GIT_TAG        20.40.0
)
FetchContent_MakeAvailable(etl)
```

In the `fprime-examples` repository, this is done in [ExternalLibs/CMakeLists.txt](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs/CMakeLists.txt) file. This allows any module within the ExternalLibs module to use `etl` library.
If the library is only needed by a single component, it is also acceptable to place the `FetchContent` code directly in that component's `CMakeLists.txt` file. If the library is needed at the project level, it is best to place the `FetchContent` code in the root `project.cmake` file.

##### Step 2: Set the library as a dependency of the module

Any component or module that depends on ETL can now link against it by adding the following entry to its `MOD_DEPS`:

```cmake
# In MyComponent/CMakeLists.txt 
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MyComponent.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/MyComponent.cpp"
)
set(MOD_DEPS
  etl::etl   # This is the target name for the ETL library
)
register_fprime_module()
```

---

## Approach 3: Install once and integrate with find_package()

|     |     |
| --- | --- |
| **Benefits** | Easier than approach 4 |
| **Drawbacks** | Additional step managed outside of CMake |
| **Considerations** | Need to ensure developers will install the dependency |

Some libraries may not be built with CMake but still integrate with CMake through [installed config files](https://cmake.org/cmake/help/v3.31/guide/using-dependencies/index.html). In this case, you may choose to install the library manually on your machine and use [`find_package()`](https://cmake.org/cmake/help/v3.31/command/find_package.html) in CMake to locate it. This is a common approach for libraries that have complex build requirements.

##### Step 1: Install the library on your system

There can be many ways to install the library, such as using a package manager (e.g., `apt`, `brew`, `yum`, `conda`, etc.), or building from source. Refer to each the library installation instructions to decide which fits your project best.

##### Step 2: Use find_package() to locate the library

CMake will look for installed packages in standard locations as well as additional locations based on your PATH. Should you install the library in a non-standard location, you may need to set the [`CMAKE_PREFIX_PATH`](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html) variable to point to the directory containing the library's CMake configuration files (in the form `<libName>Config.cmake` or `<libName>-config.cmake`).

The [ImageProcessor component](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs/ImageProcessor) component demonstrates this approach by encapsulating the OpenCV library after it has been installed on the system. The following code shows how to integrate the OpenCV library using `find_package()`:

```cmake
# The following requires that OpenCV is installed on the system and may need to be added to CMAKE_MODULE_PATH
find_package(OpenCV REQUIRED)
set(MOD_DEPS
  ${OpenCV_LIBS} # You may only need a subset of the libraries
)
register_fprime_module()
```

The `find_package()` command searches for the OpenCV library and sets the `OpenCV_LIBS` variable to the appropriate libraries. You may refer to your own library's documentation to find the correct variable or direct library names to use.  

> [!IMPORTANT]
> When using this approach, you must ensure that users of your project have the library installed on their system. This can be done by providing instructions in your project's documentation.

---

## Approach 4: Build from source alongside your project with ExternalProject_Add

|     |     |
| --- | --- |
| **Benefits** | High control over the environment |
| **Drawbacks** | Can be very hard to set-up correctly; builds only at project build-time |
| **Considerations** | Sometimes it's the only way... |

> [!NOTE]
> This section is using the OpenSSL library to illustrate. However, should you need OpenSSL in your project, note that OpenSSL is also available through the find_package() API mentioned above.

The ExternalProject module is highly flexible and allows you to build external projects as part of your CMake project, regardless of their build process. The full documentation can be found here: [https://cmake.org/cmake/help/v3.31/module/ExternalProject.html](https://cmake.org/cmake/help/v3.31/module/ExternalProject.html).

#### Step 1: Understand the library's build process

Before you can use `ExternalProject_Add`, you need to understand how the library is built. This includes knowing the build system it uses (e.g., CMake, Makefile, etc.), any dependencies it may have, any configuration options that need to be set, as well as where the installation artifacts are placed.

#### Step 2: Use ExternalProject_Add to build the library within CMake

The library source code must be made available to the ExternalProject_Add command. This can be done with a local path (e.g. a git submodule or local tarball) or by specifying a remote URL. 

The `ExternalProject_Add` command is used to build the library as part of your CMake project. This is done by specifying the steps identified in Step 1 through the `CONFIGURE_COMMAND`, `BUILD_COMMAND`, `INSTALL_COMMAND`, and more. Please referer to the [ExternalProject documentation](https://cmake.org/cmake/help/v3.31/module/ExternalProject.html) for more information. 

After this is done, you can create a CMake library using `add_library()` and set its properties to point to the built library and its include directories. This allows you to use the library in your F´ modules just like any other library.

The following example demonstrates how to integrate the OpenSSL library using `ExternalProject_Add`:

```cmake

include(ExternalProject)
set(OPENSSL_SOURCE_DIR ${FPRIME_PROJECT_ROOT}/lib/openssl)
set(OPENSSL_INSTALL_DIR ${CMAKE_BINARY_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_INSTALL_DIR}/include)
set(OPENSSL_LIBCRYPTO_LIB ${OPENSSL_INSTALL_DIR}/lib/libcrypto.a)

ExternalProject_Add(
    OpenSSL
    SOURCE_DIR ${OPENSSL_SOURCE_DIR}
    CONFIGURE_COMMAND
        ${OPENSSL_SOURCE_DIR}/config
        --prefix=${OPENSSL_INSTALL_DIR}
        --openssldir=${OPENSSL_INSTALL_DIR}
        --libdir=lib # this makes sure libcrypto.a is in lib/ (instead of lib64 on some systems)
        no-tests
    BUILD_COMMAND make -j8 build_generated libcrypto.a
    TEST_COMMAND ""
    INSTALL_COMMAND make install_sw
    INSTALL_DIR ${OPENSSL_INSTALL_DIR}
    BUILD_BYPRODUCTS ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBCRYPTO_LIB} # allows the build to know libcrypto.a is a byproduct of this ExternalProject_Add
)
# Since ExternalProject_Add will only build OpenSSL at project-build-time, we need to create the
# include directory so it exists at project-configure-time
file(MAKE_DIRECTORY ${OPENSSL_INCLUDE_DIR})
# Declare the properties needed for the OpenSSL::Crypto library
add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_LIBCRYPTO_LIB})
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
add_dependencies(OpenSSL::Crypto OpenSSL)

```

> [!NOTE]
> The source code for this example can be found at [ExternalLibs/OpenSslWrapper](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs/CMakeLists.txt).


#### Step 3: Set the library as a dependency of the module/component

Any component or module that depends on OpenSSL can now link against it by adding the following entry to its `MOD_DEPS`:

```cmake
# OpenSslWrapper component
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/OpenSslWrapper.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/OpenSslWrapper.cpp"
)
set(MOD_DEPS
  OpenSSL::Crypto   # Target name we just registered for the OpenSSL crypto library
)
register_fprime_module()
```

> [!NOTE]
> The source code for this example can be found at [ExternalLibs/OpenSslWrapper](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs/OpenSslWrapper).

---

## Patterns for using a library in the F´ architecture

When integrating a library into your F´ project, you may want to consider how the library will be used within the F´ architecture. The following patterns are common when using libraries in F´:

### 1. Wrapper Component

A wrapper component is a component that encapsulates the library's functionality and provides an interface to the library functionalities for other F´ components to use through port calls. This allows for other components of the system to benefit from the library functionalities directly through F Prime objects. In the [ExternalLibs examples](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ExternalLibs/) that we have seen so far, we have used this pattern to wrap the OpenSSL cryptographic functions in the `OpenSslWrapper` component so that we encrypt and decrypt Fw::Buffer objects directly, through a port call. This also allows to centralize the management of cryptographic keys and other configuration parameters in the wrapper component, making it easier to manage and update the library's usage across the system.

### 2. Global library usage

You may wish to use library code in multiple places, without wrapping it in a component. This can be the case for libraries that provide utilities that are not tied to a specific component or unit of behavior. This can for example be the case for the [ETL library](https://github.com/ETLCPP/etl) – used in both `OpenSslWrapper`  and `OpenCvWrapper` – which provides a set of utilities for working with C++ containers and algorithms in embedded systems. In this case, each component that needs to use the library can define it in their `MOD_DEPS` for direct usage.

### 3. Others

It is ultimately up to the project to determine how to best use a library within the F´ architecture. The above patterns are common, but you may find that your project requires a different approach based on the library's functionality and how it fits into your system's architecture.
