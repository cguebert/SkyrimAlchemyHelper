cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "Skyrim Alchemy Helper")
project(${PROJECT_NAME})

set(EXTLIB_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs)
set(EXTLIB_INCLUDE_DIR ${EXTLIB_BASE_DIR}/include)
set(EXTLIB_BINARIES_DIR ${EXTLIB_BASE_DIR}/lib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(TARGET_DIR ${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE})

# Multiprocessor build on Visual C++
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

include_directories("libs")

add_subdirectory("libs/parser")
add_subdirectory("libs/saveParser")
add_subdirectory("libs/modParser")
add_subdirectory("gui")
