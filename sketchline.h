#ifndef SKETCHLINE_H
#define SKETCHLINE_H

#include "sketchmesh.h"
#include <QVector3D>
#include <QObject>

/**
 * @brief The SketchLine class
 * Cuboid mesh that represents the lines in 3d
 */
class SketchLine : public SketchMesh
{
public:
    SketchLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint);
    QList<QVector3D> getVertices();
    QList<QList<int>> getFaces();

    /**
     * @brief radius
     * The radius represent the half of the square side
     * at the extremities of the joint
     */
    static constexpr const float radius = 10.0;

    /**
     * @brief edgeShortcut
     * Edge shortcut configures how much distance
     * are part of a joint and thus removed from a beam
     */
    static constexpr const float edgeShortcut = 40.0;
private:
    QList<QVector3D> vertices;
    QList<QList<int>> faces;
};

#endif // SKETCHLINE_H
