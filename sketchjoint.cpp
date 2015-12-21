#include "sketchjoint.h"
#include "sketchline.h"
#include <QDebug>

SketchJoint::SketchJoint(QObject* rawPoint, QList<QObject*> lines) {

    /**
      * Precondition : note that this is only possible to compute
      * if the angle between each line is greater than
      * Arcsin(SketchLine::radius/SketchLine::edgeShortcut) otherwise
      * we would need to cut more than SketchLine::edgeShortcut
      *
      * 1) Sort the line with respect to angle from the rawPoint center
      * 2) Compute two point on the line equation at distance parametrized
      *    via SketchLine::radius  and compute two point on the line
      * 3) compute the intersection
      *    from following equation : http://stackoverflow.com/a/385355
      *
      *    float x12 = x1 - x2;
      *    float x34 = x3 - x4;
      *    float y12 = y1 - y2;
      *    float y34 = y3 - y4;
      *
      *    float c = x12 * y34 - y12 * x34;
      *
      *    if (fabs(c) < 0.01)
      *    {
      *      // No intersection
      *      return false;
      *    }
      *    else
      *    {
      *      // Intersection
      *      float a = x1 * y2 - y1 * x2;
      *      float b = x3 * y4 - y3 * x4;
      *
      *      float x = (a * x34 - b * x12) / c;
      *      float y = (a * y34 - b * y12) / c;
      *
      *      return true;
      *    }
      *
      * 4) We build the list of vertices in this way :
      *     foreach Lines
      *     I.   take the first point on the line drawn by third and fourth point on line 1
      *     II.  take the intersection between the current and next line
      *     III. take the first point on the line drawn by first and sercond point on line 2
      * 5) Compute the 3D vertex
      */


    /*
     * 1)
     */

    qDebug() << "new SketchJoint";
    qDebug() << lines;
    qDebug() << rawPoint;
    qDebug() << "edgeShortcut" << SketchLine::edgeShortcut;
    qDebug() << "radius" << SketchLine::radius;

    QVector2D point = rawPoint->property("start").value<QVector2D>();

    QVector2D horizon = QVector2D(1,0);
    QList<QVector2D> externalLines;

    // Nothing to do if no more than one line
    if(lines.size() <= 1) {
        return;
    }

    foreach(QObject* line, lines) {
        QVector2D pointer = line->property("pointer").value<QVector2D>().normalized();
        if(line->property("endPoint").value<QObject*>() == rawPoint) {
            //qDebug() << "return the pointer";
            pointer *= -1.0f;
        }

        float angle = qRadiansToDegrees(qAtan2(pointer.y(),pointer.x()));
        // want angle from 0 to 360
        if(angle < 0) {
            angle = 360 + angle;
        }

        line->setProperty("sortAngle", angle);
        qDebug() << "line #" << line->property("identifier").toInt() << ":" << angle;

        /*
         * 2) compute the two points
         */

        // need the normal in the inverse direction of angle
        QVector2D normal = QVector2D(pointer.y(), -pointer.x()).normalized();

        QVector2D firstPoint = point + (normal * SketchLine::radius);
        QVector2D secondPoint = point + (pointer * SketchLine::edgeShortcut) + (normal * SketchLine::radius);

        QVector2D thirdPoint = point - (normal * SketchLine::radius);
        QVector2D fourthPoint = point + (pointer * SketchLine::edgeShortcut) - (normal * SketchLine::radius);

        qDebug() << "#:" <<  line->property("identifier").toInt();

        qDebug() << "  -> origin" << point.x() << point.y() <<  "";
        qDebug() << "  -> normal" << normal <<  "";
        qDebug() << "  -> normal * radius" << (normal * SketchLine::radius) <<  "\n";
        qDebug() << "  -> pointer" << pointer <<  "";
        qDebug() << "  -> pointer * edgeShortcut" << (pointer * SketchLine::edgeShortcut) <<  "\n";

        qDebug() << " -> firstPoint (" << firstPoint.x() << "," << firstPoint.y() << ")";
        qDebug() << " -> secondPoint (" << secondPoint.x() << "," << secondPoint.y() << ")";

        qDebug() << " -> thirdPoint (" << thirdPoint.x() << "," << thirdPoint.y() << ")";
        qDebug() << " -> fourthPoint (" << fourthPoint.x() << "," << fourthPoint.y() << ")";

        line->setProperty("firstPoint", firstPoint);
        line->setProperty("secondPoint", secondPoint);
        line->setProperty("thirdPoint", thirdPoint);
        line->setProperty("fourthPoint", fourthPoint);
    }

    std::sort(lines.begin(), lines.end(), [](const QObject* line1, const QObject* line2) -> bool {
        float angle1 = line1->property("sortAngle").toFloat();
        float angle2 = line2->property("sortAngle").toFloat();
        return angle1 < angle2;
    });

    // 3) Compute the intersection
    int i = 0;
    foreach(QObject* line, lines) {
        qDebug() << "#" <<  i << ":" <<  line->property("identifier").toInt();
        i++;
    }

    // List of vertices of the joint form
    QList<QVector2D> vertices;

    float preconditionAngle = qAsin(SketchLine::radius/SketchLine::edgeShortcut);

    for(int lineIndex = 0; lineIndex < lines.size(); lineIndex++) {

        /**
         * Line 1 is characterized by A = (x1,y1) and B = (x2,y2)
         * Line 2 is characterized by C = (x3,y3) and D = (x4,y4)
         */

        QObject* line1 = lines.at(lineIndex);
        QObject* line2 = lines.at((lineIndex + 1) % lines.size());

        qDebug() << "#" <<  i << ":" <<  line1->property("identifier").toInt();

        /*
         * Before computing, we check for the precondition on angle
         */
        float deltaAngle = line2->property("sortAngle").toFloat() - line1->property("sortAngle").toFloat();

        if(deltaAngle < 0) {
            deltaAngle += 360;
        }

        if(deltaAngle < preconditionAngle) {
            qDebug("Impossible to compute the joint because bean are too closer in term of angle");
            return;
        }

        QVector2D A = line1->property("thirdPoint").value<QVector2D>();
        QVector2D B = line1->property("fourthPoint").value<QVector2D>();

        QVector2D C = line2->property("firstPoint").value<QVector2D>();
        QVector2D D = line2->property("secondPoint").value<QVector2D>();

        float x1 = A.x();
        float y1 = A.y();

        float x2 = B.x();
        float y2 = B.y();

        float x3 = C.x();
        float y3 = C.y();

        float x4 = D.x();
        float y4 = D.y();

        float x12 = x1 - x2;
        float x34 = x3 - x4;
        float y12 = y1 - y2;
        float y34 = y3 - y4;

        float c = x12 * y34 - y12 * x34;

        if (fabs(c) < 0.01) {
          // No intersection
          qDebug() << "no Intersection found";
          return;
        }

        // Intersection
        float a = x1 * y2 - y1 * x2;
        float b = x3 * y4 - y3 * x4;

        float x = (a * x34 - b * x12) / c;
        float y = (a * y34 - b * y12) / c;

        QVector2D intersection = QVector2D(x, y);

        vertices << B;
        vertices << intersection;
        vertices << D;
    }
    qDebug() << "\n---------";
    qDebug() << "vertices\n";

    int faceIndex = 0;
    int totalFaces = vertices.size();
    QList<int> topFace;
    QList<int> bottomFace;
    QList<QVector3D> topVertices;
    foreach(QVector2D vertex, vertices) {
        qDebug() << "(" << vertex.x() << "," << vertex.y() << ")";

        this->vertices << QVector3D(vertex.x(), vertex.y(), SketchLine::radius);
        topVertices << QVector3D(vertex.x(), vertex.y(), -SketchLine::radius);

        topFace << faceIndex;
        bottomFace << faceIndex + totalFaces;

        QList<int> sideFace;
        sideFace << faceIndex
                 << (faceIndex + totalFaces)
                 << (((faceIndex + 1) % totalFaces) + totalFaces)
                 << ((faceIndex + 1) % totalFaces);

        this->faces << sideFace;

        faceIndex++;
    }

    this->vertices << topVertices;
    this->faces << topFace;
    this->faces << bottomFace;

    qDebug() << "---------\n";


}

QList<QVector3D> SketchJoint::getVertices() {
    qDebug() << "get vertices" << vertices;
    return vertices;
}

QList<QList<int>> SketchJoint::getFaces() {
    qDebug() << "get faces" << faces;
    return faces;
}


