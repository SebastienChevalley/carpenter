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


RESOURCES += qml.qrc

# enable debug
DEFINES += CARPENTER_DEBUG
# enable using SketchPoint as joint component
#DEFINES += CARPENTER_USE_SKETCHPOINT
# enable using SketchJoint as joint component
DEFINES += CARPENTER_USE_SKETCHJOINT

macx {
    INCLUDEPATH += /usr/local/include/

    LIBS += /usr/local/lib/libassimp.dylib
}
linux {
    INCLUDEPATH += /usr/local/include/

    LIBS += -L/usr/local/lib
    LIBS += -lassimp
}
win32  {
    # build or download binaries from : https://bruceoutdoors.wordpress.com/2014/08/22/building-assimp-3-with-mingw-4-8/
    INCLUDEPATH += C:\Assimp3-1-1_MinGW4-8-1_Win32\include

    LIBS += -LC:\Assimp3-1-1_MinGW4-8-1_Win32\ -lassimp
}
android {

}


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    qmldir

#QMAKE_CC=/usr/local/Cellar/gcc/5.2.0/bin/g++-5
#QMAKE_CXX=/usr/local/Cellar/gcc/5.2.0/bin/g++-5
#QMAKE_LINK=/usr/local/Cellar/gcc/5.2.0/bin/g++-5
