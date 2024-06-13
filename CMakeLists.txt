cmake_minimum_required(VERSION 3.15)

# cross compile for universal macOS binaries
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
# set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "" FORCE)

# set(CMAKE_C_COMPILER_version "3.4")
# set(CMAKE_CXX_COMPILER_version "3.4")

PROJECT(SINENSIS
LANGUAGES CXX
VERSION 0.0.2
)

# for clang-tidy(this enable to find system header files).
if(CMAKE_EXPORT_COMPILE_COMMANDS)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# IF(APPLE)
#   SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
# ENDIF(APPLE)

# SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)


# Enable JUCE. Do not use find_package to prevent from mix up with one globally installed.
add_subdirectory(lib/JUCE)

add_subdirectory(src)

# /opt/homebrew/bin/cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -S/Users/thomas/PROJETS/JUCE_SINENSIS -B/Users/thomas/PROJETS/JUCE_SINENSIS/build -G Ninja