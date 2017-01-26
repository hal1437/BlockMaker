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
    CadEditForm.cpp \
    SmartDimension.cpp \
    SmartDimensionDialog.cpp \
    Restraint.cpp \
    ExportDialog.cpp \
    CBoxDefineDialog.cpp \
    CObject/CArc.cpp \
    CObject/CLine.cpp \
    CObject/CObject.cpp \
    CObject/CPoint.cpp \
    CObject/CRect.cpp \
    CObject/CSpline.cpp \
    CObject/CBlock.cpp

HEADERS  += MainWindow.h \
    Point.h \
    CadEditForm.h \
    Utils.h \
    SmartDimension.h \
    SmartDimensionDialog.h \
    Restraint.h \
    ExportDialog.h \
    CBoxDefineDialog.h \
    CObject/CArc.h \
    CObject/CBoxDefineDialog.h \
    CObject/CLine.h \
    CObject/CObject.h \
    CObject/CPoint.h \
    CObject/CRect.h \
    CObject/CSpline.h \
    CObject/CBlock.h

FORMS    += MainWindow.ui \
    CadEditForm.ui \
    SmartDimensionDialog.ui \
    ExportDialog.ui \
    CBoxDefineDialog.ui

RESOURCES += \
    images.qrc
