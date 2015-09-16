cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "ModParser")
project(${PROJECT_NAME})

set(HEADER_FILES
	BSAFile.h
	Config.h
	ConfigParser.h
	ContainersParser.h
	Mod.h
	ModParser.h
	StringsTable.h
)

set(SOURCE_FILES
	BSAFile.cpp
	ConfigParser.cpp
	ContainersParser.cpp
	Mod.cpp
	ModParser.cpp
	StringsTable.cpp
)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

target_link_libraries(${PROJECT_NAME} "Parser")

include_directories(${EXTLIB_INCLUDE_DIR})

add_library("ZLib" STATIC IMPORTED)
set_target_properties("ZLib" PROPERTIES IMPORTED_LOCATION "${EXTLIB_BINARIES_DIR}/zlibstatic.lib")
target_link_libraries(${PROJECT_NAME} "ZLib")
