cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "SkyrimAlchemyHelper")
project(${PROJECT_NAME})

set(QT_BASE_DIR "" CACHE PATH "Qt5 install prefix") # C:/Qt/5/msvc2015/
list(APPEND CMAKE_PREFIX_PATH ${QT_BASE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)

set(HEADER_FILES
	Config.h
	ConfigDialog.h
	ConfigPane.h
	EffectsList.h
	EffectsListWidget.h
	EffectsSelector.h
	FiltersWidget.h
	FlowLayout.h
	GameSave.h
	IngredientsList.h
	IngredientsListWidget.h
	IngredientsSelector.h
	MainWindow.h
	PluginsList.h
	PluginsListWidget.h
	PotionsList.h
	PotionsListWidget.h
	SaveDialog.h
	SelectorWidget.h
)

set(SOURCE_FILES
	main.cpp
	Config.cpp
	ConfigDialog.cpp
	ConfigPane.cpp
	EffectsList.cpp
	EffectsListWidget.cpp
	EffectsSelector.cpp
	FiltersWidget.cpp
	FlowLayout.cpp
	GameSave.cpp
	IngredientsList.cpp
	IngredientsListWidget.cpp
	IngredientsSelector.cpp
	MainWindow.cpp
	PluginsList.cpp
	PluginsListWidget.cpp
	PotionsList.cpp
	PotionsListWidget.cpp
	SaveDialog.cpp
	SelectorWidget.cpp
)

set(RESSOURCE_FILES SAH.qrc SAH.rc)

find_package(Qt5Widgets)
find_package(Qt5OpenGL)

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES} ${RESSOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Applications")

target_link_libraries(${PROJECT_NAME} Qt5::Widgets SaveParser)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${TARGET_DIR})

# Install Qt files
set(QT_LIBS Qt5Core Qt5Gui Qt5Widgets)
set(qtLibsRelease)
set(qtLibsDebug)

foreach(libName ${QT_LIBS})
	list(APPEND qtLibsRelease	"${QT_BASE_DIR}/bin/${libName}.dll")
	list(APPEND qtLibsDebug		"${QT_BASE_DIR}/bin/${libName}d.dll")
endforeach(libName)

install(FILES ${qtLibsRelease} CONFIGURATIONS Release RelWithDebInfo DESTINATION ${TARGET_DIR})
install(FILES ${qtLibsDebug} CONFIGURATIONS Debug DESTINATION ${TARGET_DIR})

set(QT_PLUGINS qdds qgif qicns qico qjp2 qjpeg qmng qsvg qtga qtiff qwbmp qwebp)
set(qtPluginsRelease)
set(qtPluginsDebug)

foreach(libName ${QT_PLUGINS})
	list(APPEND qtPluginsRelease	"${QT_BASE_DIR}/plugins/imageformats/${libName}.dll")
	list(APPEND qtPluginsDebug		"${QT_BASE_DIR}/plugins/imageformats/${libName}d.dll")
endforeach(libName)

install(FILES ${qtPluginsRelease} CONFIGURATIONS Release RelWithDebInfo DESTINATION "${TARGET_DIR}/imageformats")
install(FILES ${qtPluginsDebug} CONFIGURATIONS Debug DESTINATION "${TARGET_DIR}/imageformats")

install(FILES "${QT_BASE_DIR}/plugins/platforms/qwindows.dll" CONFIGURATIONS Release RelWithDebInfo DESTINATION "${TARGET_DIR}/platforms")
install(FILES "${QT_BASE_DIR}/plugins/platforms/qwindowsd.dll" CONFIGURATIONS Debug DESTINATION "${TARGET_DIR}/platforms")
