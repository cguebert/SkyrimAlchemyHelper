cmake_minimum_required(VERSION 3.5)

find_package(ZLIB REQUIRED)

set(target "SkyrimAlchemyHelper")
project(${target})

set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON) # Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTORCC ON) # Instruct CMake to run rcc automatically when needed.

# External dependencies
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(HEADER_FILES
	Config.h
	ConfigDialog.h
	ContainersWidget.h
	DiscoverEffects.h
	EffectsListWidget.h
	FiltersWidget.h
	FlowLayout.h
	GameSave.h
	IngredientsListWidget.h
	InventoryWidget.h
	KnownIngredientsWidget.h
	MainWindow.h
	ModsParserWrapper.h
	PluginsListWidget.h
	PotionsList.h
	PotionsListWidget.h
	SaveDialog.h
	SelectorWidget.h
	Settings.h
)

set(SOURCE_FILES
	main.cpp
	Config.cpp
	ConfigDialog.cpp
	ContainersWidget.cpp
	DiscoverEffects.cpp
	EffectsListWidget.cpp
	FiltersWidget.cpp
	FlowLayout.cpp
	GameSave.cpp
	IngredientsListWidget.cpp
	InventoryWidget.cpp
	KnownIngredientsWidget.cpp
	MainWindow.cpp
	ModsParserWrapper.cpp
	PluginsListWidget.cpp
	PotionsList.cpp
	PotionsListWidget.cpp
	SaveDialog.cpp
	SelectorWidget.cpp
	Settings.cpp
)

set(RESSOURCE_FILES SAH.qrc SAH.rc)

add_executable(${target} WIN32 ${HEADER_FILES} ${SOURCE_FILES} ${RESSOURCE_FILES})

set_target_properties(${target} PROPERTIES FOLDER "Applications")

target_link_libraries(${target}
	Qt5::Widgets
	${META_PROJECT_NAME}::SaveParser
	${META_PROJECT_NAME}::ModParser
)
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/libs)

set_target_properties(${target} PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_definitions(${target} PUBLIC ${DEFAULT_COMPILE_DEFINITIONS})
target_compile_options(${target} PUBLIC ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(${target} PUBLIC ${DEFAULT_LINKER_OPTIONS})

# 
# Deployment
# 

# Executable
install(TARGETS ${target} RUNTIME DESTINATION debug CONFIGURATIONS Debug)
install(TARGETS ${target} RUNTIME DESTINATION release CONFIGURATIONS Release)

# Copy the dependencies dlls
foreach(export_lib Qt5::Widgets Qt5::Core Qt5::Gui ZLIB::ZLIB)
	InstallDependency(${export_lib})
endforeach()

InstallDependency(Qt5::QWindowsIntegrationPlugin "/platforms")

# Copy the VC runtime
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION release CONFIGURATIONS Release)
include(InstallRequiredSystemLibraries)
