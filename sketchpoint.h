#ifndef SKETCHPOINT_H
#define SKETCHPOINT_H

#include "sketchmesh.h"
#include "sketchline.h"
#include <QVector3D>
#include <QVector2D>
#include <QObject>

/**
 * @brief The SketchPoint class
 *
 * Coarse version of the joint mesh between lines
 */
class SketchPoint : public SketchMesh
{
    public:
        SketchPoint(QObject* point);
        QList<QVector3D> getVertices();
        QList<QList<int>> getFaces();
    private:
        QList<QVector3D> vertices;
        QList<QList<int>> faces;
};

#endif // SKETCHPOINT_H
