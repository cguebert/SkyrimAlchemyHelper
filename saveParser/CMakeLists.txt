cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "SaveParser")
project(${PROJECT_NAME})

set(HEADER_FILES
	Save.h
)

set(SOURCE_FILES
	Save.cpp
)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

include_directories("../parser")

add_library("ZLib" SHARED IMPORTED)
set_target_properties("ZLib" PROPERTIES IMPORTED_IMPLIB zlib/zdll.lib)
target_link_libraries(${PROJECT_NAME} "ZLib")