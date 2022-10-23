cmake_minimum_required(VERSION 3.22.1..3.24)
project(GTANite LANGUAGES CXX)

option(USE_DOBBY_CODE_PATCH "Use Dobby code patch for KittyMemory::memWrite" OFF)

file(GLOB INCLUDE_FILES
        src/*.h
        src/utils/*.h)

file(GLOB SOURCE_FILES
        src/*.cpp)

add_library(${PROJECT_NAME} SHARED
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS "-fexceptions")

set_target_properties(${PROJECT_NAME} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

target_compile_definitions(${PROJECT_NAME} PUBLIC
        SPDLOG_FMT_EXTERNAL)

if (USE_DOBBY_CODE_PATCH)
    message(STATUS "Use Dobby code patch.")
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_DOBBY_CODE_PATCH)
else ()
    message(STATUS "Use Dobby code patch disabled.")
endif ()

message(STATUS "Using GTANite version ${PROJECT_VERSION}.")
target_compile_definitions(${PROJECT_NAME} PUBLIC
        GTANite_VERSION="${PROJECT_VERSION}")

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
add_subdirectory(vendor/Dobby)
add_subdirectory(vendor/KittyMemory)
endif ()

target_link_libraries(${PROJECT_NAME}
        android
        log
        dobby
        KittyMemory)

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan/conan.cmake"
            TLS_VERIFY ON)
endif ()

include(${CMAKE_BINARY_DIR}/conan/conan.cmake)

conan_cmake_configure(REQUIRES fmt/8.1.1 magic_enum/0.8.1 nlohmann_json/3.11.2 spdlog/1.10.0
        GENERATORS cmake_find_package
        IMPORTS "bin, * -> ./bin"
        IMPORTS "lib, * -> ./lib"
        OPTIONS OPTIONS fmt:shared=True spdlog:shared=True)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android" AND NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Android")
    # Create Conan profile for cross-compiling based on current
    # CMake information.
    include(../cmake/conan/android_cross_compiling.cmake)
    conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_BINARY_DIR}
            INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
            BUILD missing
            REMOTE conancenter
            PROFILE_BUILD default
            PROFILE_HOST "${CMAKE_BINARY_DIR}/conan/android_conan_profile")
else ()
    # For normal compiling and cross-compiling with same os but different
    # architecture.
    conan_cmake_autodetect(settings)
    conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_BINARY_DIR}
            INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
            BUILD missing
            REMOTE conancenter
            SETTINGS ${settings})
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/conan")

find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(${PROJECT_NAME}
        fmt::fmt
        magic_enum::magic_enum
        nlohmann_json::nlohmann_json
        spdlog::spdlog)
