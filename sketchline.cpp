#include "sketchline.h"
#include <QObject>
#include <QVariant>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include "QtMath"

SketchLine::SketchLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint) {
    QVariant maybePointer = line->property("pointer");
    QVariant maybeStart = line->property("start");

    if(!maybePointer.isValid() || !maybePointer.convert(QVariant::Vector2D)) {
        qDebug() << "SketchLine can't convert pointer";
        return;
    }

    if(!maybeStart.isValid() || !maybeStart.convert(QVariant::Vector2D)) {
        qDebug() << "SketchLine can't convert start";
        return;
    }

    QVector2D pointer = maybePointer.value<QVector2D>();
    QVector2D direction = pointer.normalized();
    QVector2D horizon = QVector2D(0,1);
    QVector2D start = maybeStart.value<QVector2D>();

    float angle = qRadiansToDegrees(qAtan2(pointer.y(),pointer.x()) - qAtan2(horizon.y(),horizon.x()));
    //qDebug() << "angle" << angle;

    float radius = SketchLine::radius;
    float length = pointer.length();


    bool cutStart = linesPerPoint.contains(line->property("startPoint").value<QObject*>());
    bool cutEnd = linesPerPoint.contains(line->property("endPoint").value<QObject*>());

    if(cutStart) {
        start += direction * SketchLine::edgeShortcut;
        length -= SketchLine::edgeShortcut;
    }
    if(cutEnd) {
        length -= SketchLine::edgeShortcut;
    }

    QVector3D a = QVector3D(radius, 0, radius);
    QVector3D b = QVector3D(-radius, 0, radius);
    QVector3D c = QVector3D(radius, 0, -radius);
    QVector3D d = QVector3D(-radius, 0, -radius);

    QVector3D e = QVector3D(radius, length, radius);
    QVector3D f = QVector3D(-radius, length, radius);
    QVector3D g = QVector3D(radius, length, -radius);
    QVector3D h = QVector3D(-radius, length, -radius);

    QList<QVector3D> allVertices;
    allVertices << a << b << c << d << e << f << g << h;

    QMatrix4x4 transform;
    transform.rotate(angle, 0, 0, 1);
    QVector4D translate = start.toVector4D();
    translate.setW(1);

    transform.setColumn(3, translate);
    //transform.translate(start.toVector3D());

    for(int i = 0; i < 8; i++)
    {
        allVertices.replace(i, transform * allVertices.at(i));
        //allVertices[i] += start.toVector3D();

        vertices << allVertices[i];
        //qDebug() << "insert vertex" << allVertices[i];
    }

    int facesIndex[36] = {
        0,1,2,
        1,2,3,
        0,1,4,
        1,4,5,
        0,2,4,
        2,4,6,
        2,3,7,
        2,6,7,
        1,3,5,
        3,5,7,
        4,5,6,
        5,6,7
    };

    for(int i = 0; i < 12; i++) {
        QList<int> face;
        face << facesIndex[i*3] << facesIndex[i*3+1] << facesIndex[i*3+2];
        faces << face;
        //qDebug() << "insert faces" << QVector3D(facesIndex[i], facesIndex[i+1], facesIndex[i+2]);
    }

    //qDebug()<< "#faces" << faces.size();
}

QList<QVector3D> SketchLine::getVertices() {
    return vertices;
}

QList<QList<int>> SketchLine::getFaces() {
    return faces;
}
