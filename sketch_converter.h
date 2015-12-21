
#ifndef SKETCHCONVERTER_H
#define SKETCHCONVERTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVector2D>
#include <QVariant>
#include <QVariantMap>
#include <QMap>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/Exporter.hpp>
#include <sketchmesh.h>
#include <sketchpoint.h>
#include <sketchline.h>
#include <sketchjoint.h>
#include <assimp/postprocess.h>



class SketchConverter : public QObject
{
    Q_OBJECT
    public:
        SketchConverter();

        //static ColladaFile fromSketch(Sketch sketch);

    private:
        void addPoint(QObject* point);
        void addLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint);
        void addJoint(QObject* point, QList<QObject*> lines);
        QList<SketchMesh*> meshes;

    public slots:
        void exportToFile(QObject* sketch, QString path);
};

#endif // SKETCHCONVERTER_H


