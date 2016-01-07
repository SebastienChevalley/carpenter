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
    QVariant maybeStartPoint = line->property("startPoint");
    QVariant maybeEndPoint = line->property("endPoint");

    if(!maybePointer.isValid() || !maybePointer.canConvert<QVector2D>()) {
        this->setErrorMessage("Line doesn't contain a pointer");
        return;
    }

    if(!maybeStart.isValid() || !maybeStart.canConvert<QVector2D>()) {
        this->setErrorMessage("Line doesn't contain a start vector");
        return;
    }

    if(!maybeStartPoint.isValid() || !maybeStartPoint.canConvert<QObject*>()) {
        this->setErrorMessage("Line doesn't contain a start point");
        return;
    }

    if(!maybeEndPoint.isValid() || !maybeEndPoint.canConvert<QObject*>()) {
#ifdef CARPENTER_DEBUG
        qDebug() << "SketchLine: endPoint " << "isValid:" << maybeEndPoint.isValid();
#endif
        this->setErrorMessage("Line doesn't contain an ending point");
        return;
    }

    QVector2D pointer = maybePointer.value<QVector2D>();
    QVector2D direction = pointer.normalized();
    QVector2D horizon = QVector2D(0,1);
    QVector2D start = maybeStart.value<QVector2D>();

    QObject* startPoint = maybeStartPoint.value<QObject*>();
    QObject* endPoint = maybeEndPoint.value<QObject*>();

    float angle = qRadiansToDegrees(qAtan2(pointer.y(),pointer.x()) - qAtan2(horizon.y(),horizon.x()));
    //qDebug() << "angle" << angle;

    float radius = SketchLine::radius;
    float length = pointer.length();


    bool cutStart = linesPerPoint.contains(startPoint);
    bool cutEnd = linesPerPoint.contains(endPoint);

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

    for(int i = 0; i < 8; i++) {
        allVertices.replace(i, transform * allVertices.at(i));

        vertices << allVertices[i];

#ifdef CARPENTER_DEBUG
        qDebug() << "SketchLine: insert vertex" << allVertices[i];
#endif
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

#ifdef CARPENTER_DEBUG
        qDebug() << "SketchLine: insert faces" << QVector3D(facesIndex[i], facesIndex[i+1], facesIndex[i+2]);
#endif
    }
#ifdef CARPENTER_DEBUG
    qDebug()<< "SketchLine: #faces" << faces.size();
#endif

    this->setValid(true);
}

QList<QVector3D> SketchLine::getVertices() {
    return vertices;
}

QList<QList<int>> SketchLine::getFaces() {
    return faces;
}
