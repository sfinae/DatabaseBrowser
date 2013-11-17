
TEMPLATE = lib
CONFIG += plugin
CONFIG += debug

TARGET = DataViewer

DESTDIR = ../../plugins

QT += sql widgets

INCLUDEPATH += ./GeneratedFiles \
		.
DEPENDPATH += ./GeneratedFiles	\
		.

INCLUDEPATH += ./../../DatabaseBrowser

MOC_DIR += ./GeneratedFiles
OBJECTS_DIR += ./GeneratedFiles
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

#Include file(s)
include(PluginExample.pri)
