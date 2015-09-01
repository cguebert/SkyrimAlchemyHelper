cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "SkyrimAlchemyHelper")
project(${PROJECT_NAME})

set(QT_BASE_DIR "" CACHE PATH "Qt5 install prefix") # C:/Qt/5/msvc2015/
set(QT_SUFFIX "54" CACHE STRING "Qt libs suffix") # 54 for Qt 5.4
list(APPEND CMAKE_PREFIX_PATH ${QT_BASE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(HEADER_FILES
	ConfigDialog.h
	ConfigPane.h
	EffectsList.h
	EffectsListModel.h
	EffectsListWidget.h
	EffectsSelector.h
	FiltersWidget.h
	FlowLayout.h
	IngredientsList.h
	IngredientsListModel.h
	IngredientsListWidget.h
	IngredientsSelector.h
	MainWindow.h
	PluginsList.h
	PluginsListModel.h
	PluginsListWidget.h
	PotionsList.h
	PotionsListModel.h
	PotionsListWidget.h
	SelectorWidget.h
)

set(SOURCE_FILES
	main.cpp
	ConfigDialog.cpp
	ConfigPane.cpp
	EffectsList.cpp
	EffectsListModel.cpp
	EffectsListWidget.cpp
	EffectsSelector.cpp
	FiltersWidget.cpp
	FlowLayout.cpp
	IngredientsList.cpp
	IngredientsListModel.cpp
	IngredientsListWidget.cpp
	IngredientsSelector.cpp
	MainWindow.cpp
	PluginsList.cpp
	PluginsListModel.cpp
	PluginsListWidget.cpp
	PotionsList.cpp
	PotionsListModel.cpp
	PotionsListWidget.cpp
	SelectorWidget.cpp
)

set(RESSOURCE_FILES SAH.qrc SAH.rc)

find_package(Qt5Widgets)
find_package(Qt5OpenGL)

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES} ${RESSOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Applications")

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${TARGET_DIR})

# Install Qt files
set(QT_LIBS Qt5Core Qt5Gui Qt5Widgets)
set(QT_ADDITIONAL_LIBS icudt icuin icuuc)

set(qtLibsRelease)
set(qtLibsDebug)

foreach(libName ${QT_LIBS})
	list(APPEND qtLibsRelease	"${QT_BASE_DIR}/bin/${libName}.dll")
	list(APPEND qtLibsDebug		"${QT_BASE_DIR}/bin/${libName}d.dll")
endforeach(libName)

foreach(libName ${QT_ADDITIONAL_LIBS})
	list(APPEND qtLibsRelease	"${QT_BASE_DIR}/bin/${libName}${QT_SUFFIX}.dll")
	list(APPEND qtLibsDebug		"${QT_BASE_DIR}/bin/${libName}${QT_SUFFIX}.dll")
endforeach(libName)

install(FILES ${qtLibsRelease} CONFIGURATIONS Release RelWithDebInfo DESTINATION ${TARGET_DIR})
install(FILES ${qtLibsDebug} CONFIGURATIONS Debug DESTINATION ${TARGET_DIR})

install(FILES "${QT_BASE_DIR}/plugins/platforms/qwindows.dll" CONFIGURATIONS Release RelWithDebInfo DESTINATION "${TARGET_DIR}/plugins/platforms")
install(FILES "${QT_BASE_DIR}/plugins/platforms/qwindowsd.dll" CONFIGURATIONS Debug DESTINATION "${TARGET_DIR}/plugins/platforms")
