# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = VerilogIncludeImport
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui
CONFIG += c++11 release
DEFINES += QT_DLL QT_WIDGETS_LIB VERILOGINCLUDEIMPORT_LIB QT_XML_LIB
INCLUDEPATH += ./../.. \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug
LIBS += -L"./../../executable" \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(VerilogIncludeImport.pri)

target.path = $$plugin_path
INSTALLS += target
