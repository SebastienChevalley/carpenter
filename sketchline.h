#ifndef SKETCHLINE_H
#define SKETCHLINE_H

#include "sketchmesh.h"
#include <QVector3D>
#include <QObject>


class SketchLine : public SketchMesh
{
public:
    SketchLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint);
    QList<QVector3D> getVertices();
    QList<QList<int>> getFaces();
    static constexpr const float radius = 10.0;
    static constexpr const float edgeShortcut = 40.0;
    static constexpr const float minimalLength = 20.1;
private:
    QList<QVector3D> vertices;
    QList<QList<int>> faces;
};

#endif // SKETCHLINE_H
