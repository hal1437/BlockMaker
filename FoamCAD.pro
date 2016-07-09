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

SOURCES += main.cpp\
        MainWindow.cpp \
    CObject.cpp \
    CPoint.cpp \
    CLine.cpp \
    CadEditForm.cpp \
    SmartDimension.cpp \
    CArc.cpp \
    SmartDimensionDialog.cpp \
    Restraint.cpp \
    CSpline.cpp \
    CRect.cpp \
    ExportDialog.cpp \
    CBlocks.cpp \
    CBoxDefineDialog.cpp

HEADERS  += MainWindow.h \
    CObject.h \
    CPoint.h \
    Relative.h \
    Point.h \
    CLine.h \
    CadEditForm.h \
    Utils.h \
    SmartDimension.h \
    CArc.h \
    SmartDimensionDialog.h \
    Restraint.h \
    CSpline.h \
    CRect.h \
    ExportDialog.h \
    CBlocks.h \
    CBoxDefineDialog.h

FORMS    += MainWindow.ui \
    CadEditForm.ui \
    SmartDimensionDialog.ui \
    ExportDialog.ui \
    CBoxDefineDialog.ui

RESOURCES += \
    images.qrc
