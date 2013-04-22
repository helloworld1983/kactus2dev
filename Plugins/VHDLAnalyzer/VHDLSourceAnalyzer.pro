# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = VHDLSourceAnalyzer
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui help
CONFIG += release
DEFINES += VHDLSOURCEANALYZER_LIB QT_XML_LIB QT_XMLPATTERNS_LIB QT_DLL QT_HELP_LIB QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/$(Configuration) \
    . \
    ./../.. \
    $(QTDIR)/../qttools/include \
    $(QTDIR)/../qttools/include/QtHelp
LIBS += -L"./../../executable" \
    -lKactus2
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(VHDLSourceAnalyzer.pri)
OTHER_FILES += vhdlPlugin.json

target.path = /usr/share/kactus2/plugins
INSTALLS += target