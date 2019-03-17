cmake_minimum_required(VERSION 3.5)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# main app configuration
option(LD_ANDROID_BUILD "Build for Android" OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# lib

# try to set QTDIR from environment variable
if(EXISTS $ENV{QTDIR})
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

# android
if(NOT DEFINED LD_ANDROID_BUILD)
    set(LD_ANDROID_BUILD OFF)
endif()

# load local
include(local.cmake OPTIONAL)

# set desktop qt dir
if(DEFINED QT_BASE_DIR AND NOT DEFINED QTDIR)
    if(LD_ANDROID_BUILD )
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(QTDIR "${QT_BASE_DIR}/android_armv7")
        elseif(ANDROID_ABI STREQUAL "x86")
            set(QTDIR "${QT_BASE_DIR}/android_x86")
        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(QTDIR "${QT_BASE_DIR}/android_arm64_v8a")
        endif()
    else()
        # win
        if(WIN32)
            set(QTDIR "${QT_BASE_DIR}/msvc2017")
        endif()
        # mac
        if(APPLE)
            set(QTDIR "${QT_BASE_DIR}/clang_64")
        endif()
    endif()
endif()
message(STATUS "Qt Dir: ${QTDIR}")

# Android toolchain: should be before project() call
if(LD_ANDROID_BUILD)
    # setup required values for toolchain files if not done already
    if(NOT DEFINED ANDROID_NDK AND EXISTS $ENV{ANDROID_NDK_ROOT})
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    endif()

    set(ANDROID_NATIVE_API_LEVEL 21)
    set(ANDROID_TOOLCHAIN clang)
    set(ANDROID_STL c++_shared)

    # setup Android toolchain
    set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

message(STATUS "Qt Dir: ${QTDIR}")

# setup Qt_DIR variables (required for android)
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
set(Qt5Gui_DIR "${QTDIR}/lib/cmake/Qt5Gui")
set(Qt5Multimedia_DIR "${QTDIR}/lib/cmake/Qt5Multimedia")
set(Qt5Network_DIR "${QTDIR}/lib/cmake/Qt5Network")
set(Qt5OpenGL_DIR "${QTDIR}/lib/cmake/Qt5OpenGL")
set(Qt5Qml_DIR "${QTDIR}/lib/cmake/Qt5Qml")

# project
project(LaserdockGames)

# library
option(LD_CORE_BUILD_EXAMPLE "" OFF)
option(LD_CORE_ENABLE_QT_QUICK "" ON)
add_subdirectory(core)

option(LD_GAMES_BUILD_EXAMPLE "" ON)
add_subdirectory(games)
