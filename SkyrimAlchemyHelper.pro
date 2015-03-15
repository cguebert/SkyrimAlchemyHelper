#-------------------------------------------------
#
# Project created by QtCreator 2015-03-14T15:40:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SkyrimAlchemyHelper
TEMPLATE = app


SOURCES += \ 
    src/EffectsList.cpp \
    src/EffectsListModel.cpp \
    src/EffectsListWidget.cpp \
    src/IngredientsList.cpp \
    src/IngredientsListModel.cpp \
    src/IngredientsListWidget.cpp \
    src/main.cpp \
    src/PluginsList.cpp \
    src/PluginsListModel.cpp \
    src/PluginsListWidget.cpp \
    src/SAHDialog.cpp

HEADERS  += \ 
    src/EffectsList.h \
    src/EffectsListModel.h \
    src/EffectsListWidget.h \
    src/IngredientsList.h \
    src/IngredientsListModel.h \
    src/IngredientsListWidget.h \
    src/PluginsList.h \
    src/PluginsListModel.h \
    src/PluginsListWidget.h \
    src/SAHDialog.h

DISTFILES += \
    SAH.rc

RESOURCES += \
    SAH.qrc
win32:RC_FILE = SAH.rc
