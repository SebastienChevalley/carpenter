#ifndef SKETCHJOINT_H
#define SKETCHJOINT_H

#include "sketchmesh.h"
#include <QList>
#include <QObject>
#include <QVariant>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include "QtMath"


class SketchJoint : public SketchMesh
{
public:
    SketchJoint(QObject* point, QList<QObject*> lines);

    QList<QVector3D> getVertices();
    QList<QList<int>> getFaces();
private:
    QList<QVector3D> vertices;
    QList<QList<int>> faces;
};

#endif // SKETCHJOINT_H
