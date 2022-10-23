cmake_minimum_required(VERSION 3.22.1..3.24)
project(KittyMemory)

set(KITTYMEMORY_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KITTYMEMORY_MAIN_PROJECT ON)
endif ()

option(KITTYMEMORY_DEBUG "Enable KittyMemory debug log" OFF)

set(INCLUDE_FILES
        ClearCache.h
        KittyArm64.h
        KittyMemory.h
        KittyScanner.h
        KittyUtils.h
        MemoryBackup.h
        MemoryPatch.h)

set(SOURCE_FILES
        ClearCache.c
        KittyArm64.cpp
        KittyMemory.cpp
        KittyScanner.cpp
        KittyUtils.cpp
        MemoryBackup.cpp
        MemoryPatch.cpp)

add_library(${PROJECT_NAME} STATIC
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

if (KITTYMEMORY_DEBUG)
    target_compile_definitions(${PROJECT_NAME} PUBLIC KITTYMEMORY_DEBUG)
endif ()

target_include_directories(${PROJECT_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
