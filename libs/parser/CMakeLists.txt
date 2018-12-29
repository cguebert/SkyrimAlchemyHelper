cmake_minimum_required(VERSION 3.5)

set(target "Parser")
project(${target})

set(HEADER_FILES
	Parser.h
)

set(SOURCE_FILES
	Parser.cpp
)

add_library(${target} STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})  # Create namespaced alias

set_target_properties(${target} PROPERTIES FOLDER "Libraries")
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/libs)

set_target_properties(${target} PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_definitions(${target} PUBLIC ${DEFAULT_COMPILE_DEFINITIONS})
target_compile_options(${target} PUBLIC ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(${target} PUBLIC ${DEFAULT_LINKER_OPTIONS})