#ifndef SKETCHMESH_H
#define SKETCHMESH_H

#include <QList>
#include <QVector3D>

class SketchMesh
{
public:
    virtual QList<QVector3D> getVertices() = 0;
    virtual QList<QList<int>> getFaces() = 0;

};

#endif // SKETCHMESH_H
