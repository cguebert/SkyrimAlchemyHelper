cmake_minimum_required(VERSION 3.5)

find_package(ZLIB REQUIRED)
find_package(LZ4 REQUIRED)

set(target "SaveParser")
project(${target})

set(HEADER_FILES
	Save.h
)

set(SOURCE_FILES
	Save.cpp
)

add_library(${target} STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})  # Create namespaced alias

set_target_properties(${target} PROPERTIES FOLDER "Libraries")
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(${target} ${META_PROJECT_NAME}::Parser ZLIB::ZLIB LZ4)

set_target_properties(${target} PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_definitions(${target} PUBLIC ${DEFAULT_COMPILE_DEFINITIONS})
target_compile_options(${target} PUBLIC ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(${target} PUBLIC ${DEFAULT_LINKER_OPTIONS})
