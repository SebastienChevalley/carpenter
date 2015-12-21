#include "sketchpoint.h"
#include "QtMath"
#include <QObject>
#include <QVector3D>
#include <QVariant>
#include <QVector2D>
#include <QDebug>

SketchPoint::SketchPoint(QObject* point)
{
    QVariant maybeStart = point->property("start");

    if(!maybeStart.isValid() || !maybeStart.convert(QVariant::Vector2D)) {
        qDebug() << "SketchPoint: start not valid";
        return;
    }

    QVector2D position = maybeStart.value<QVector2D>();
    qDebug() << "SketchPoint @" << position;

    float length = 10.0f;
    QVector3D translate = QVector3D(position.x(), position.y(), -length / 2.0f);

    int numberOfSides = 40;
    float angle = 360.f / numberOfSides;
    float radius = 5.0f;

    int numberOfVertices = numberOfSides * 2 + 2;
    int numberOfFaces = numberOfSides * 4;

    //std::vector<glm::vec3> cylinderVertices;


    for(int i = 0; i < numberOfVertices; i++) {
        float height = i >= numberOfSides ^ i == numberOfSides * 2 ? length : 0.f;
        if(i >= numberOfSides * 2) {
            //cylinderVertices.push_back(glm::vec3(0.f, 0.f, height));
            vertices << QVector3D(0.f, 0.f, height);
            //qDebug() << "insert vertices" << i << QVector3D(0.f, 0.f, height);
        }
        else {
            //cylinderVertices.push_back(glm::vec3(glm::cos(glm::radians(angle * (i % numberOfSides))) * radius, glm::sin(glm::radians(angle * (i % numberOfSides))) * radius, height));
            vertices << QVector3D(qCos(qDegreesToRadians(angle * (i % numberOfSides))) * radius, qSin(qDegreesToRadians(angle * (i % numberOfSides))) * radius, height);
            //qDebug() << "insert vertices" << i << QVector3D(qCos(qDegreesToRadians(angle * (i % numberOfSides))) * radius, qCos(qDegreesToRadians(angle * (i % numberOfSides))) * radius, height);
        }
    }

    for(int i = 0; i < vertices.size(); i++) {
        QVector3D current = vertices.at(i);
        //qDebug() << "SketchPoint vertex " << current;
        vertices.replace(i, QVector3D(current.x() + translate.x(), current.y() + translate.y(), current.z() + translate.z()));
    }

    int bottomCenter = numberOfSides * 2;
    int topCenter = bottomCenter + 1;

    for(int i = 0, k = 0; i < numberOfSides * 2; i++) {
        QList<int> face;
        // top and bottom
        if(i < numberOfSides) {
            face << k << ((k + 1) % numberOfSides) << bottomCenter;
            //qDebug() << "insert faces" << i << faces.last();


            if((i*2+1) < numberOfFaces) {
                face << (k + numberOfSides) << ((k + 1) % numberOfSides + numberOfSides) << topCenter;
                //qDebug() << "insert faces" << i << faces.last();

            }
        }
        // side
        else {
            if(i == numberOfSides) {
                k = 0;
            }

            face << k << (k + numberOfSides) << ((k + 1) % numberOfSides + numberOfSides);
            //qDebug() << "insert faces" << i << faces.last();


            if((i*2+1) < numberOfFaces) {
                face << k << ((k + 1) % numberOfSides) << ((k + 1) % numberOfSides + numberOfSides);
                //qDebug() << "insert faces" << i << faces.last();

            }
        }

        faces << face;


        k++;
    }


}

QList<QVector3D> SketchPoint::getVertices() {
    return vertices;
}

QList<QList<int>> SketchPoint::getFaces() {
    return faces;
}


