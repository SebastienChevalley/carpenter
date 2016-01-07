#ifndef SKETCHMESH_H
#define SKETCHMESH_H

#include <QList>
#include <QVector3D>
#include <QString>
#include <QDebug>

/**
 * @brief The SketchMesh class
 *
 * Represents a 3d mesh with vertices and faces.
 * Contains also methods for validation and error
 * handling
 */
class SketchMesh
{
public:
    virtual QList<QVector3D> getVertices() = 0;
    virtual QList<QList<int>> getFaces() = 0;
    virtual ~SketchMesh() {
#ifdef CARPENTER_DEBUG
        qDebug() << "SketchMesh: mesh cleared";
#endif
    }

    bool isValid();
    QString error();
    void setValid(bool valid);
    void setErrorMessage(QString message);
    QString getErrorMessage();

private:
    bool valid;
    QString errorMessage;
};

#endif // SKETCHMESH_H
