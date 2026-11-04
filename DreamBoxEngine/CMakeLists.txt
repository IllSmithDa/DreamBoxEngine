cmake_minimum_required(VERSION 3.24)

project(
    DreamBoxEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put executables and dynamic libraries in the same location.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# -------------------------
# Dependencies
# -------------------------

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

# -------------------------
# DreamBox Engine
# -------------------------

add_executable(
    DreamBoxEngine

    src/DreamBoxEngine.cpp
    src/Engine.cpp
    src/Input.cpp
    src/Player.cpp
    src/GameObject.cpp
    src/Wall.cpp
    src/Collision.cpp

    tests/CollisionTests.cpp

    include/DreamBox/Engine.h
    include/DreamBox/Input.h
    include/DreamBox/Player.h
    include/DreamBox/GameObject.h
    include/DreamBox/Wall.h
    include/DreamBox/Collision.h

 "include/DreamBox/GameObject.h" "src/GameObject.cpp")

target_include_directories(
    DreamBoxEngine
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    DreamBoxEngine
    PRIVATE
    SDL3::SDL3
)

target_link_libraries(
    CollisionTests
    PRIVATE
    SDL3::SDL3
)

add_test(
    NAME CollisionTests
    COMMAND CollisionTests
)