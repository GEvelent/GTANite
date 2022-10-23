if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
        set(CONAN_ANDROID_ARCH armv7)
    elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
        set(CONAN_ANDROID_ARCH armv8)
    elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        set(CONAN_ANDROID_ARCH x86)
    elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86_64")
        set(CONAN_ANDROID_ARCH x86_64)
    else ()
        message(FATAL_ERROR "Invalid Android ABI: ${CMAKE_ANDROID_ARCH_ABI}.")
    endif()

    file(WRITE ${CMAKE_BINARY_DIR}/conan/android_conan_profile
"[settings]
arch=${CONAN_ANDROID_ARCH}
build_type=${CMAKE_BUILD_TYPE}
compiler=${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION}
compiler.libcxx=${CMAKE_ANDROID_STL_TYPE}
compiler.version=12
os=${CMAKE_SYSTEM_NAME}
os.api_level=${CMAKE_SYSTEM_VERSION}
[conf]
tools.cmake.cmaketoolchain:generator=${CMAKE_GENERATOR}
tools.cmake.cmaketoolchain:user_toolchain=[\"${CMAKE_BINARY_DIR}/conan/user_android_conan.toolchain.cmake\"]
tools.android:ndk_path=${CMAKE_ANDROID_NDK}
[tool_requires]
[options]
[env]
CONAN_CMAKE_GENERATOR=${CMAKE_GENERATOR}
CONAN_CMAKE_TOOLCHAIN_FILE=${CMAKE_BINARY_DIR}/conan/android_conan.toolchain.cmake
")

    # Replace \\ to / in CMAKE_MAKE_PROGRAM.
    string(REPLACE "\\" "." CMAKE_MAKE_PROGRAM_ ${CMAKE_MAKE_PROGRAM})

    # If the recipes doesn't create CMakeToolchain.
    file(WRITE ${CMAKE_BINARY_DIR}/conan/android_conan.toolchain.cmake
"set(ANDROID_PLATFORM android-${CMAKE_SYSTEM_VERSION})
set(ANDROID_STL ${CMAKE_ANDROID_STL_TYPE})
set(ANDROID_ABI ${CMAKE_ANDROID_ARCH_ABI})
if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL \"armeabi-v7a\")
    set(CMAKE_ANDROID_ARM_MODE ${CMAKE_ANDROID_ARM_MODE})
    set(CMAKE_ANDROID_ARM_NEON ${CMAKE_ANDROID_ARM_NEON})
endif ()
set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM_} CACHE FILEPATH \"\" FORCE)
include(${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake)
unset(CMAKE_EXPORT_NO_PACKAGE_REGISTRY)
")

    # The recipes create CMakeToolchain.
    file(WRITE ${CMAKE_BINARY_DIR}/conan/user_android_conan.toolchain.cmake
"if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL \"armeabi-v7a\")
    set(CMAKE_ANDROID_ARM_MODE ${CMAKE_ANDROID_ARM_MODE})
    set(CMAKE_ANDROID_ARM_NEON ${CMAKE_ANDROID_ARM_NEON})
endif ()
set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM_} CACHE FILEPATH \"\" FORCE)
")
endif()
