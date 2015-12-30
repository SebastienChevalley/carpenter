#include "sketchconstraintssolver.h"

#include <QDebug>
#include <QVariant>
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include "solve.h"
using namespace cv;

SketchConstraintsSolver::SketchConstraintsSolver(QObject *parent) : QObject(parent) {

}

void SketchConstraintsSolver::setSketch(QObject *sketch) {
    qDebug() << "my own setter";
    this->sketch = sketch;
    this->solved = false;
}

QObject* SketchConstraintsSolver::getSketch() {
    return this->sketch;
}

QVariant SketchConstraintsSolver::solve() {
    this->solved = false;
    if(this->sketch == Q_NULLPTR) {
        return "Sketch is null";
    }

    /*
     *  get the mm/pixel scale
     */
    QVariant isScaleSet;
    bool isIsMmPerPixelScaleSetCallWorks = QMetaObject::invokeMethod(
                this->sketch,
                "isMmPerPixelScaleSet",
                Q_RETURN_ARG(QVariant, isScaleSet)
    );

    if(!isIsMmPerPixelScaleSetCallWorks) {
        return "Cannot check if the scale is set";
    }

    if(!isScaleSet.toBool()) {
        return "Cannot apply constraints without scale";
    }

    QVariant mmPerPixelScale;
    bool isGetMmPerPixelScaleCallWorks = QMetaObject::invokeMethod(
                this->sketch,
                "getMmPerPixelScale",
                Q_RETURN_ARG(QVariant, mmPerPixelScale)
    );

    if(!isGetMmPerPixelScaleCallWorks) {
        return "Cannot retrieve the scale";
    }

    double pixelPerMmScale = 1.0 / mmPerPixelScale.toDouble();


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

    int horizontalAndVerticalConstraints = 0;

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
            horizontalAndVerticalConstraints++;
        }

        if(vertical) {
            cLine1->verticallyConstrained();
            cLine2->verticallyConstrained();
            horizontalAndVerticalConstraints++;
        }

        this->lines << cLine1 << cLine2;
    }

    /*
     * Solve vertical and horizontal constraints
     */
    QSet<ConstrainedPoint*> visited;
    QQueue<ConstrainedPoint*> waitingList;

    waitingList += this->points[0];
    first = true;

    int visitedEdge = 0;
    int appliedConstraints = 0;
    while(!waitingList.isEmpty()) {
        ConstrainedPoint* current;

        current = waitingList.dequeue();

        qDebug() << "current: " << *(current->x.data()) << "," << *(current->y.data());

        visited += current;

        foreach(ConstrainedLine* line, this->lines) {
            if(!visited.contains(line->end) && !waitingList.contains(line->end)) {
                waitingList.enqueue(line->end);
            }

            if(line->end == current && visited.contains(line->start)) {
                visitedEdge++;


                // it's the first point thus the origin
                if(first) {
                    first = false;
                    /*(!current->tryToSetX(current->x)) {
                        return "Cannot set origin";
                    }
                    if(!current->tryToSetY(current->y)) {
                        return "Cannot set origin";
                    }*/
                }

                if(line->isHorizontalConstrained()) {
                    appliedConstraints++;
                    qDebug() << "try to constrain horizontally:" << line->end->x <<","<<line->end->y;
                    if(!current->tryToSetY(line->start->y)) {
                        return "Conflict on horizontal constraints";
                    }
                }

                if(line->isVerticallyConstrained()) {
                    appliedConstraints++;
                    qDebug() << "try to constrain vertically:" << line->end->x <<","<<line->end->y;
                    if(!current->tryToSetX(line->start->x)) {
                        return "Conflict on vertical constraints";
                    }
                }
            }
        }
    }

    /*
     * apply distance constraints
     *
     * 1. separate the parameters between free and non-free one and create
     *    point datastructure for the solver
     * 2. count the constraints needed
     * 3. create constraints datastructure
     * 4.
     */
    QList<double*> parameters;
    SketchSolvePoint solvePoints[this->points.size()];
    Line solveLines[this->lines.size()];
    int totalConstraints = this->constraints.size() + horizontalAndVerticalConstraints;
    Constraint constraints[totalConstraints];
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

    double* pparameters[parameters.size()];
    QSet<int> identifiersSeen;
    QList<double> lengthParameters;

    i = 0;
    int constraintIndex = 0;
    foreach(ConstrainedLine* line, this->lines) {
        if(!identifiersSeen.contains(line->identifier)) {
            identifiersSeen += line->identifier;

            solveLines[i].p1 = solvePoints[identifierToSolvePointsIndex[line->start->identifier()]];
            solveLines[i].p2 = solvePoints[identifierToSolvePointsIndex[line->end->identifier()]];

            if(line->isDistanceFixed()) {
                constraints[constraintIndex].line1 = solveLines[i];
                lengthParameters += line->desiredDistance * pixelPerMmScale;
                constraints[constraintIndex].type = lineLength;
                constraints[constraintIndex].parameter = &lengthParameters.last();

                constraintIndex++;
            }

            if(line->isHorizontalConstrained()) {
                constraints[constraintIndex].line1 = solveLines[i];
                constraints[constraintIndex].type = sketchSolveHorizontal;

                constraintIndex++;
            }

            if(line->isVerticallyConstrained()) {
                constraints[constraintIndex].line1 = solveLines[i];
                constraints[constraintIndex].type = sketchSolveVertical;

                constraintIndex++;
            }

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
    qDebug() << "constraints: " << this->constraints.size();
    qDebug() << "parameters: " << parameters.size();

    qDebug() << "solver";
    qDebug() << "------";
    qDebug() << "constraintsIndex:" << constraintIndex;
    qDebug() << "total constraints:" << totalConstraints;


    int result = ::solve(pparameters, parameters.size(), constraints, totalConstraints, fine);

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
        return "Solution not found";
    }
    else {
        return "Unexpected result";
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
