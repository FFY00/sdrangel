#--------------------------------------------------------
#
# Pro file for Android and Windows builds with Qt Creator
#
#--------------------------------------------------------

TEMPLATE = lib
CONFIG += plugin

QT += core gui widgets multimedia

TARGET = modam

DEFINES += USE_SSE2=1
QMAKE_CXXFLAGS += -msse2
DEFINES += USE_SSE4_1=1
QMAKE_CXXFLAGS += -msse4.1
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD
INCLUDEPATH += ../../../sdrbase
INCLUDEPATH += ../../../sdrgui

CONFIG(Release):build_subdir = release
CONFIG(Debug):build_subdir = debug

SOURCES += ammod.cpp\
	ammodgui.cpp\
	ammodplugin.cpp\
	ammodsettings.cpp

HEADERS += ammod.h\
	ammodgui.h\
	ammodplugin.h\
	ammodsettings.h

FORMS += ammodgui.ui

LIBS += -L../../../sdrbase/$${build_subdir} -lsdrbase
LIBS += -L../../../sdrgui/$${build_subdir} -lsdrgui

RESOURCES = ../../../sdrgui/resources/res.qrc
