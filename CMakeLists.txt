cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "Skyrim Alchemy Helper")
project(${PROJECT_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(TARGET_DIR ${CMAKE_INSTALL_PREFIX}/\${BUILD_TYPE})

# Multiprocessor build on Visual C++
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_subdirectory("parser")
#add_subdirectory("modParser")
add_subdirectory("gui")
