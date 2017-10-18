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
    src/Restraint.cpp \
    src/FoamFile.cpp \
    src/CObject/CPoint.cpp \
    src/CObject/CObject.cpp \
    src/CObject/CEdge.cpp \
    src/CObject/CLine.cpp \
    src/CObject/CArc.cpp \
    src/CObject/CSpline.cpp \
    src/CObject/CBlock.cpp \
    src/Dialog/ExportDialog.cpp \
    src/Dialog/GridFilter.cpp \
    src/Dialog/GridFilterDialog.cpp \
    src/Dialog/MoveTransformDialog.cpp \
    src/Dialog/SmartDimension.cpp \
    src/Dialog/SmartDimensionDialog.cpp \
    src/Dialog/PropertyDefinitionDialog.cpp \
    src/CadModelCore.cpp \
    src/SolidEditForm.cpp \
    src/TimeDivider.cpp \
    src/SolidEditController.cpp \
    src/Collision.cpp \
    src/MakeObjectController.cpp \
    src/CObject/CFace.cpp \
    src/ObjectList.cpp \
    src/CadModelMenu.cpp \
    src/CObject/CFileEdge.cpp \
    src/CadModelSearch.cpp \
    src/CObject/CStl.cpp

HEADERS  += src/MainWindow.h \
    src/Point.h \
    src/Utils.h \
    src/Restraint.h \
    src/FoamFile.h \
    src/CObject/CPoint.h \
    src/CObject/CObject.h \
    src/CObject/CEdge.h \
    src/CObject/CLine.h \
    src/CObject/CArc.h \
    src/CObject/CSpline.h \
    src/CObject/CBlock.h \
    src/Dialog/ExportDialog.h \
    src/Dialog/GridFilter.h \
    src/Dialog/GridFilterDialog.h \
    src/Dialog/MoveTransformDialog.h \
    src/Dialog/SmartDimension.h \
    src/Dialog/SmartDimensionDialog.h \
    src/Dialog/PropertyDefinitionDialog.h \
    src/CadModelCore.h \
    src/SolidEditForm.h \
    src/TimeDivider.h \
    src/SolidEditController.h \
    src/Collision.h \
    src/MakeObjectController.h \
    src/CObject/CFace.h \
    src/ObjectList.h \
    src/CadModelMenu.h \
    src/CObject/CFileEdge.h \
    src/CadModelSearch.h \
    src/CObject/CStl.h

FORMS    += src/MainWindow.ui \
    src/Dialog/SmartDimensionDialog.ui \
    src/Dialog/ExportDialog.ui \
    src/Dialog/MoveTransformDialog.ui \
    src/Dialog/GridFilterDialog.ui \
    src/Dialog/PropertyDefinitionDialog.ui \
    src/SolidEditForm.ui

RESOURCES += \
    Resource/images.qrc

#MAC限定
macx {
    DEFINES += MACOSX
}
#Unix限定
unix:!macx {
    DEFINES += UNIX
    LIBS += -lGLU
}


