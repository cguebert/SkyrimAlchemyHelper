cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "ModParser")
project(${PROJECT_NAME})

set(HEADER_FILES
	BSAFile.h
	Config.h
	ConfigParser.h
	Mod.h
	ModParser.h
	StringsTable.h
)

set(SOURCE_FILES
	BSAFile.cpp
	ConfigParser.cpp
	Mod.cpp
	ModParser.cpp
	StringsTable.cpp
)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

target_link_libraries(${PROJECT_NAME} "Parser")
