#include "sketchconstraintssolver.h"

#include <QDebug>
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include "solve.h"
using namespace cv;

SketchConstraintsSolver::SketchConstraintsSolver(QObject *parent) : QObject(parent)
{

}

void SketchConstraintsSolver::setSketch(QObject *sketch) {
    qDebug() << "sketch:" << sketch;
    this->solved = false;
    this->sketch = sketch;
}

bool SketchConstraintsSolver::solve() {
    this->solved = false;
    qDebug() << "please solve";
    if(this->sketch == Q_NULLPTR) {
        qDebug() << "sketch is null";
    }

    // clean old computation
    this->points.clear();
    this->lines.clear();
    this->constraints.clear();
    bool first = false;

    // extract sketch data
    QVariantMap store = this->sketch->property("store").value<QVariantMap>();
    QVariantList points = store["points"].value<QVariantList>();
    QVariantList lines = store["lines"].value<QVariantList>();

    foreach(QVariant item, points) {
        QObject* point = item.value<QObject*>();
        if(first) {
            this->origin = point->property("start").value<QVector2D>();
            first = false;
        }

        QVector2D start = point->property("start").value<QVector2D>();

        ConstrainedPoint* honk = new ConstrainedPoint(
            start.x() - this->origin.x(),
            start.y() - this->origin.y(),
            point
        );

        this->points[point->property("identifier").toInt()] = honk;
    }

    // create the constrained lines
    foreach(QVariant item, lines) {
        QObject* line = item.value<QObject*>();

        int start = line->property("startPoint").value<QObject*>()->property("identifier").toInt();
        int end = line->property("endPoint").value<QObject*>()->property("identifier").toInt();

        bool vertical = line->property("verticallyConstrained").toBool();
        bool horizontal = line->property("horizontallyConstrained").toBool();

        bool distanceFixed = line->property("distanceFixed").toBool();

        int identifier = line->property("identifier").toInt();

        ConstrainedLine* cLine1 = new ConstrainedLine(this->points[start], this->points[end], identifier);
        ConstrainedLine* cLine2 = new ConstrainedLine(this->points[end], this->points[start], identifier);

        this->points[start]->increaseConstraintAmount();
        this->points[end]->increaseConstraintAmount();

        if(distanceFixed) {
            double desiredDistance = line->property("desiredDistance").toDouble();

            cLine1->setDesiredDistance(desiredDistance);
            cLine2->setDesiredDistance(desiredDistance);

            this->constraints << cLine1;
        }

        if(horizontal) {
            cLine1->horizontallyConstrained();
            cLine2->horizontallyConstrained();
        }

        if(vertical) {
            cLine1->verticallyConstrained();
            cLine2->verticallyConstrained();
        }

        this->lines << cLine1 << cLine2;
    }

    // DFS to solve
    QSet<ConstrainedPoint*> visited;
    QQueue<ConstrainedPoint*> waitingList;

    waitingList += this->points[0];
    first = true;

    int visitedEdge = 0;
    int appliedConstraints = 0;
    while(!waitingList.isEmpty()/* || visited.size() != this->points.size()*/) {
        ConstrainedPoint* current;

        //if(!waitingList.empty()) {
            current = waitingList.dequeue();
        //}
        /*else {
            QSet<ConstrainedPoint*> nonVisited = QSet<ConstrainedPoint*>::fromList(this->points.values());
            nonVisited -= visited;

            current = nonVisited.toList().first();
        }*/
        qDebug() << "current: " << *(current->x.data()) << "," << *(current->y.data());
        visited += current;

        foreach(ConstrainedLine* line, this->lines) {
            if(!visited.contains(line->end) && !waitingList.contains(line->end)) {
                waitingList.enqueue(line->end);
            }

            //qDebug() << "line->end == current" << (line->end == current) << ", visited.contains" << (visited.contains(line->start));

            if(line->end == current && visited.contains(line->start)) {
                visitedEdge++;

                //this->lines.removeAll(line);

                // it's the first point thus the origin
                if(first) {
                    first = false;
                    //current->tryToSetX(current->x);
                    //current->tryToSetY(current->y);
                }

                if(line->isHorizontalConstrained()) {
                    appliedConstraints++;
                    qDebug() << "try to constrain horizontally:" << line->end->x <<","<<line->end->y;
                    if(!current->tryToSetY(line->start->y)) {
                        return false;
                    }
                }

                if(line->isVerticallyConstrained()) {
                    appliedConstraints++;
                    qDebug() << "try to constrain vertically:" << line->end->x <<","<<line->end->y;
                    if(!current->tryToSetX(line->start->x)) {
                        return false;
                    }
                }
            }
        }
    }

    /*
     * apply distance constraints
     *
     * 1. separate the parameters between free and non-free one
     * 2. collect constraints objects
     */


    // FIXME VLA length
    QList<double*> parameters;
    SketchSolvePoint solvePoints[100];
    Line solveLines[100];
    Constraint constraints[100];
    double *pparameters[parameters.size()];
    QMap<QString, int> identifierToSolvePointsIndex;

    int i = 0;
    foreach(ConstrainedPoint* point, this->points) {
        if(!point->fixedX) {
            parameters.append(point->x.data());
        }
        if(!point->fixedY) {
            parameters.append(point->y.data());
        }
        solvePoints[i].x = point->x.data();
        solvePoints[i].y = point->y.data();

        identifierToSolvePointsIndex.insert(point->identifier(), i);

        i++;
    }

    int constraintsCount = 0;
    QSet<int> identifiersSeen;

    foreach(ConstrainedLine* line, this->lines) {
        if(line->isDistanceFixed() && !identifiersSeen.contains(line->identifier)) {
            constraintsCount++;
            identifiersSeen.insert(line->identifier);
        }
    }

    identifiersSeen.clear();

    i = 0;
    foreach(ConstrainedLine* line, this->lines) {
        if(line->isDistanceFixed() && !identifiersSeen.contains(line->identifier)) {
            qDebug() << "index for p1 identifier["
                     << line->start->identifier()
                     << "] = "
                     << identifierToSolvePointsIndex[line->start->identifier()]
                     << ":";

            auto honk = solvePoints[identifierToSolvePointsIndex[line->start->identifier()]];
            solveLines[i].p1 = honk;
            solveLines[i].p2 = solvePoints[identifierToSolvePointsIndex[line->end->identifier()]];

            constraints[i].line1 = solveLines[i];
            constraints[i].type = lineLength;
            // ask for visibility
            constraints[i].parameter = &line->desiredDistance;

            i++;
        }
    }

    i = 0;
    foreach(double* parameter, parameters) {
        pparameters[i] = parameter;
        i++;
    }

    qDebug() << "line constraints";
    qDebug() << "----------------\n";
    qDebug() << "constraints: " << constraintsCount;
    qDebug() << "parameters: " << parameters.size();


    int result = ::solve(pparameters, parameters.size(), constraints, constraintsCount, fine);

    if(result == succsess) {
        qDebug() << "solution found";
        qDebug() << "visited edges" << visitedEdge;
        qDebug() << "applied constraints" << appliedConstraints;

        foreach(ConstrainedPoint* point, this->points) {
            qDebug() << "(" << *(point->x.data()) << "," << *(point->y.data()) << ")";
            qDebug() << point->identifier();
        }

        this->solved = true;
    }
    else if(result == noSolution) {
        qDebug() << "solution not found";
    }
    else {
        qDebug() << "unexpected things founds";
    }

    return this->solved;
}

void SketchConstraintsSolver::applyOnSketch() {
    if(!this->solved) {
        qDebug() << "the sketch wasn'tsolved";
        return;
    }

    QVariantMap map;

    foreach(ConstrainedPoint* point, this->points) {
        map.insert(point->identifier(), QVector2D(*(point->x.data()), *(point->y.data())));
    }


    qDebug() << map;

    QVariant returnedValue;
    QMetaObject::invokeMethod(this->sketch, "movePoints",
            Q_ARG(QVariant, QVariant::fromValue(map)));
}
