#-------------------------------------------------
#
# Project created by QtCreator 2015-12-19T18:07:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FoamCAD
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ./src/

SOURCES += src/main.cpp\
        src/MainWindow.cpp \
    src/CadEditForm.cpp \
    src/SmartDimension.cpp \
    src/SmartDimensionDialog.cpp \
    src/Restraint.cpp \
    src/ExportDialog.cpp \
    src/CBoxDefineDialog.cpp \
    src/CObject/CArc.cpp \
    src/CObject/CLine.cpp \
    src/CObject/CObject.cpp \
    src/CObject/CPoint.cpp \
    src/CObject/CSpline.cpp \
    src/CObject/CBlock.cpp \
    src/MoveTransformDialog.cpp \
    src/FoamFile.cpp \
    src/CObject/CEdge.cpp

HEADERS  += src/MainWindow.h \
    src/Point.h \
    src/CadEditForm.h \
    src/Utils.h \
    src/SmartDimension.h \
    src/SmartDimensionDialog.h \
    src/Restraint.h \
    src/ExportDialog.h \
    src/CBoxDefineDialog.h \
    src/CObject/CArc.h \
    src/CObject/CBoxDefineDialog.h \
    src/CObject/CLine.h \
    src/CObject/CObject.h \
    src/CObject/CPoint.h \
    src/CObject/CSpline.h \
    src/CObject/CBlock.h \
    src/MoveTransformDialog.h \
    src/FoamFile.h \
    src/CObject/CEdge.h

FORMS    += src/MainWindow.ui \
    src/CadEditForm.ui \
    src/SmartDimensionDialog.ui \
    src/ExportDialog.ui \
    src/CBoxDefineDialog.ui \
    src/MoveTransformDialog.ui

RESOURCES += \
    Resource/images.qrc
