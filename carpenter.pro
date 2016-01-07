TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    sketch_converter.cpp \
    sketchmesh.cpp \
    sketchpoint.cpp \
    sketchline.cpp \
    sketchconstraintssolver.cpp \
    constrainedpoint.cpp \
    constrainedline.cpp \
    sketchjoint.cpp \
    solve.cpp \
    derivatives.cpp \
    sketchlolexporter.cpp \
    parameter.cpp

HEADERS += sketch_converter.h \
    sketchmesh.h \
    sketchpoint.h \
    sketchline.h \
    sketchconstraintssolver.h \
    constrainedpoint.h \
    constrainedline.h \
    sketchjoint.h \
    solve.h \
    sketchlolexporter.h \
    parameter.h

INCLUDEPATH += /usr/local/include/

RESOURCES += qml.qrc

# provide manually directory -L/usr/local/lib
LIBS += /usr/local/lib/libassimp.dylib
LIBS += /usr/local/lib/libopencv_core.2.4.dylib


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    qmldir \
    empty.dae
