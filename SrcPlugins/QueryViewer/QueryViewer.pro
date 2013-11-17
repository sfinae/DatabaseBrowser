
TEMPLATE = lib
CONFIG += plugin

#your plugin name
TARGET = queryviewer

DESTDIR = ../../plugins

#add what do you want
QT += sql widgets

INCLUDEPATH += ./../../DatabaseBrowser

MOC_DIR += ./GeneratedFiles
OBJECTS_DIR += ./GeneratedFiles
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

#Include file(s)
include(PluginExample.pri)
