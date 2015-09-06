cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "Parser")
project(${PROJECT_NAME})

set(HEADER_FILES
	Parser.h
)

set(SOURCE_FILES
	Parser.cpp
)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")
