cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "ModParser")
project(${PROJECT_NAME})

set(HEADER_FILES
	Ingredients.h
	Mod.h
	StringsTable.h
)

set(SOURCE_FILES
	main.cpp
	Ingredients.cpp
	Mod.cpp
	StringsTable.cpp
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Applications")

target_link_libraries(${PROJECT_NAME} "Parser")

include_directories("../parser")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${TARGET_DIR})