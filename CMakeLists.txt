cmake_minimum_required(VERSION 3.16.0)

# Build For ARM
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# Start Project
project(extra-api)

# Include SDK
include("$ENV{HOME}/.minecraft-pi/sdk/lib/minecraft-pi-reborn-client/sdk/sdk.cmake")

# Build
add_library(extrapi SHARED src/api.cpp src/base64.cpp src/buttons.cpp src/inventory.cpp src/world.cpp)
target_link_libraries(extrapi mods-headers reborn-patch symbols misc)
