#ifndef SKETCHMESH_H
#define SKETCHMESH_H

#include <QList>
#include <QVector3D>
#include <QString>

class SketchMesh
{
public:
    virtual QList<QVector3D> getVertices() = 0;
    virtual QList<QList<int>> getFaces() = 0;

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
