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


# -------------------------
# SDL3
# -------------------------

# NEW:
# Explicitly keep SDL3 as a shared library.
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)


# -------------------------
# SDL3_ttf
# -------------------------

# Use SDL_ttf's included dependencies.
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)

# Optional: don't build SDL_ttf sample applications.
set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)

# NEW:
# Build SDL3_ttf statically.
# This means DreamBoxEngine.exe will NOT require SDL3_ttf.dll.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
    GIT_SUBMODULES_RECURSE TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(SDL3_ttf)

# NEW:
# Restore the normal shared-library setting for anything added later.
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)


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
    src/Text.cpp
    src/SettingsScene.cpp
    src/Button.cpp
    src/Texture.cpp

    include/DreamBox/Engine.h
    include/DreamBox/Input.h
    include/DreamBox/Player.h
    include/DreamBox/GameObject.h
    include/DreamBox/Wall.h
    include/DreamBox/Collision.h
    include/DreamBox/Scene.h
    include/DreamBox/GameScene.h
    include/DreamBox/MainMenuScene.h
    include/DreamBox/Text.h
    include/DreamBox/SettingsScene.h
    include/DreamBox/Button.h
    include/DreamBox/Texture.h
)   

target_include_directories(
    DreamBoxEngine
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    DreamBoxEngine
    PRIVATE

    # SDL3 remains shared.
    SDL3::SDL3

    # SDL3_ttf::SDL3_ttf automatically points at
    # the static SDL_ttf target we created above.
    SDL3_ttf::SDL3_ttf
)


# -------------------------
# Windows Runtime DLLs
# -------------------------

if(WIN32)
    add_custom_command(
        TARGET DreamBoxEngine
        POST_BUILD

        # SDL3 is still shared, so copy SDL3.dll.
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:DreamBoxEngine>

        VERBATIM
    )
endif()


# -------------------------
# Assets
# -------------------------

if(NOT EXISTS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/Montserrat/Montserrat-Regular.ttf"
)
    message(FATAL_ERROR
        "Montserrat-Regular.ttf was not found"
    )
endif()

# Copy Montserrat font assets beside the executable.
add_custom_command(
    TARGET DreamBoxEngine
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:DreamBoxEngine>/assets/fonts/Montserrat"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/Montserrat"
        "$<TARGET_FILE_DIR:DreamBoxEngine>/assets/fonts/Montserrat"

    VERBATIM
)

# copy other assets like textures


# Copy all game assets beside the executable
add_custom_command(
    TARGET DreamBoxEngine
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:DreamBoxEngine>/assets"
    COMMENT "Copying DreamBox Engine assets"
)



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

# ==================================
# Button Tests
# ==================================

add_executable(
    ButtonTests
    ../tests/ButtonTests.cpp
)

target_include_directories(
    ButtonTests
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    ButtonTests
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_test(
    NAME ButtonTests
    COMMAND ButtonTests
)