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

# Fetch SDL text library 

set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)

# Fetch SDL 
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

# Fetch SDL Font second 

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
    GIT_SUBMODULES_RECURSE TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3_ttf)


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
    src/GameScene.cpp
    src/MainMenuScene.cpp

    include/DreamBox/Engine.h
    include/DreamBox/Input.h
    include/DreamBox/Player.h
    include/DreamBox/GameObject.h
    include/DreamBox/Wall.h
    include/DreamBox/Collision.h
    include/DreamBox/Scene.h
    include/DreamBox/GameScene.h
    include/DreamBox/MainMenuScene.h
)

target_include_directories(
    DreamBoxEngine
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    DreamBoxEngine
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

if(WIN32)
    add_custom_command(
        TARGET DreamBoxEngine
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3-shared>
            $<TARGET_FILE_DIR:DreamBoxEngine>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_ttf-shared>
            $<TARGET_FILE_DIR:DreamBoxEngine>

        VERBATIM
    )
endif()

# -------------------------
# Collision Tests
# -------------------------

add_executable(
    CollisionTests
    ../tests/CollisionTests.cpp
    src/GameObject.cpp
    src/Collision.cpp
)

target_include_directories(
    CollisionTests
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
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