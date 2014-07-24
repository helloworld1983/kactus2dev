# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = QuartusPinImportPlugin
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui
CONFIG += plugin release
DEFINES += QT_DLL QUARTUSPINIMPORTPLUGIN_LIB QT_WIDGETS_LIB QT_XML_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    $(QTDIR)/../qtxmlpatterns/include/QtXmlPatterns \
    $(QTDIR)/../qtxmlpatterns/include \
    $(QTDIR)/../qttools/include \
    ./../..
LIBS += -L"./../../executable" \
    -lKactus2 \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(QuartusPinImportPlugin.pri)

target.path = /usr/share/kactus2/plugins
INSTALLS += target
PadsPartGeneratorPadsPartGenerator
