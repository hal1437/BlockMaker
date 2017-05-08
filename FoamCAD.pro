#-------------------------------------------------
#
# Project created by QtCreator 2015-12-19T18:07:50
#
#-------------------------------------------------

QT       += core gui opengl

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
    src/CObject/CPoint.cpp \
    src/CObject/CObject.cpp \
    src/CObject/CEdge.cpp \
    src/CObject/CLine.cpp \
    src/CObject/CArc.cpp \
    src/CObject/CSpline.cpp \
    src/CObject/CBlock.cpp \
    src/MoveTransformDialog.cpp \
    src/FoamFile.cpp \
    src/SolidView.cpp \
    src/GridFilter/GridFilter.cpp \
    src/GridFilter/GridFilterDialog.cpp

HEADERS  += src/MainWindow.h \
    src/Point.h \
    src/CadEditForm.h \
    src/Utils.h \
    src/SmartDimension.h \
    src/SmartDimensionDialog.h \
    src/Restraint.h \
    src/ExportDialog.h \
    src/CBoxDefineDialog.h \
    src/CObject/CPoint.h \
    src/CObject/CObject.h \
    src/CObject/CEdge.h \
    src/CObject/CLine.h \
    src/CObject/CArc.h \
    src/CObject/CSpline.h \
    src/CObject/CBlock.h \
    src/CObject/CBoxDefineDialog.h \
    src/MoveTransformDialog.h \
    src/FoamFile.h \
    src/SolidView.h \
    src/GridFilter/GridFilter.h \
    src/GridFilter/GridFilterDialog.h

FORMS    += src/MainWindow.ui \
    src/CadEditForm.ui \
    src/SmartDimensionDialog.ui \
    src/ExportDialog.ui \
    src/CBoxDefineDialog.ui \
    src/MoveTransformDialog.ui \
    src/GridFilter/GridFilterDialog.ui

RESOURCES += \
    Resource/images.qrc
