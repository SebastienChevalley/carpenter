
#ifndef SKETCHCONVERTER_H
#define SKETCHCONVERTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVector2D>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QSharedPointer>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <sketchmesh.h>
#include <sketchpoint.h>
#include <sketchline.h>
#include <sketchjoint.h>
#include <assimp/postprocess.h>

/**
 * @brief The SketchConverter class
 *
 * It represents the 3d exporter module. You pass it
 * a sketch and a filesystem path and it create a 3d
 * file on the path
 */
class SketchConverter : public QObject
{
    Q_OBJECT
    public:
        SketchConverter();

        /**
         * @brief exportToFile
         * @param sketch Sketch qml component
         * @param path Path where the DAE file will be saved
         * @param error Error message,
         * @return true iif export works
         */
        bool exportToFile(QObject *sketch, QString path, QString& error);
    private:
        QSharedPointer<SketchLine> addLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint);
        QSharedPointer<SketchJoint> addJoint(QObject* point, QList<QObject*> lines);
        QSharedPointer<SketchPoint> addPoint(QObject* point);
        QSharedPointer<aiScene> generateScene(double scale);
        QList<QSharedPointer<SketchMesh>> meshes;

    public slots:
        /**
         * @brief exportToFile
         * @param sketch Sketch qml component
         * @param path Path where the DAE file will be saved
         * @return a bool true if the export works, a QString instead
         * with the error message. Used is just an overloading of
         * exportToFileQObject *sketch, QString path, QString error)
         * that allows to work in Javascript side, since string are
         * immutable type in Javascript.
         */
        QVariant exportToFile(QObject* sketch, QString path);
};

#endif // SKETCHCONVERTER_H


