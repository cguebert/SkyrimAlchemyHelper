cmake_minimum_required(VERSION 3.5)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(META_PROJECT_NAME "SkyrimAlchemyHelper")
project(${META_PROJECT_NAME})

option(BUILD_SHARED_LIBS "Build shared libraries." ON)

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compiler settings and options
include(cmake/CompileOptions.cmake)
include(cmake/Custom.cmake) # Some useful functions

add_subdirectory("libs/parser")
add_subdirectory("libs/saveParser")
add_subdirectory("libs/modParser")
add_subdirectory("gui")
