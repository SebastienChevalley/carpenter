#include "sketchconstraintssolver.h"

#include <QDebug>
#include <QVariant>
#include "solve.h"

SketchConstraintsSolver::SketchConstraintsSolver(QObject *parent) : QObject(parent) {

}

void SketchConstraintsSolver::setSketch(QObject *sketch) {
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

        QSharedPointer<ConstrainedPoint> newPoint(new ConstrainedPoint(
            start.x() - this->origin.x(),
            start.y() - this->origin.y(),
            point
        ));

        this->points[newPoint->identifier().toInt()] = newPoint;
    }

    int horizontalAndVerticalConstraints = 0;

    // create the constrained lines
    foreach(QVariant item, lines) {
        QObject* line = item.value<QObject*>();

        int start = line->property("startPoint").value<QObject*>()->property("identifier").toInt();
        int end = line->property("endPoint").value<QObject*>()->property("identifier").toInt();

#ifdef CARPENTER_DEBUG
        qDebug() << "SketchSolver: line [" << start << "," << end << "]";
#endif

        bool vertical = line->property("verticallyConstrained").toBool();
        bool horizontal = line->property("horizontallyConstrained").toBool();

        bool distanceFixed = line->property("distanceFixed").toBool();

        int identifier = line->property("identifier").toInt();

        QSharedPointer<ConstrainedLine> cLine1(new ConstrainedLine(this->points[start], this->points[end], identifier));
        QSharedPointer<ConstrainedLine> cLine2(new ConstrainedLine(this->points[end], this->points[start], identifier));

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

    if(this->points.size() == 0) {
        return "No point in the sketch";
    }

    /*
     * Solve vertical and horizontal constraints
     */
    QSet<QSharedPointer<ConstrainedPoint>> visited;
    QQueue<QSharedPointer<ConstrainedPoint>> waitingList;

    waitingList.enqueue(this->points.values().first());

    int visitedEdge = 0;
    int appliedConstraints = 0;
    /*while(!waitingList.isEmpty()) {
        QSharedPointer<ConstrainedPoint> current;

        current = waitingList.dequeue();

#ifdef CARPENTER_DEBUG
        qDebug() << "SketchSolver: current: " << current->x().data()->value() << "," << current->y().data()->value() << "id:" << current->identifier();
#endif

        visited += current;

        foreach(QSharedPointer<ConstrainedLine> line, this->lines) {
            if(line->start == current && !visited.contains(line->end) && !waitingList.contains(line->end)) {
                waitingList.enqueue(line->end);
            }

            if(line->end == current && visited.contains(line->start)) {
                visitedEdge++;

                if(line->isHorizontalConstrained()) {
                    appliedConstraints++;
#ifdef CARPENTER_DEBUG
                    qDebug() << "SketchSolver: try to constrain horizontally:" << line->end->x().data()->value() <<","<< line->end->y().data()->value();
#endif
                    current->setY(line->start->y());
                }

                if(line->isVerticallyConstrained()) {
                    appliedConstraints++;
#ifdef CARPENTER_DEBUG
                    qDebug() << "SketchSolver: try to constrain vertically:" << line->end->x().data()->value() <<","<< line->end->y().data()->value();
#endif
                    current->setX(line->start->x());
                }
            }
        }
    }*/

    /*
     * apply distance constraints
     *
     * 1. separate the parameters between free and non-free one and create
     *    point datastructure for the solver
     * 2. count the constraints needed
     * 3. create constraints datastructure
     * 4.
     */
    QList<QSharedPointer<double>> parameters;
    SketchSolvePoint* solvePoints = new SketchSolvePoint[this->points.size()];
    Line* solveLines = new Line[this->lines.size() /2]; // duplicate them in early stage
    int totalConstraints = this->constraints.size() + horizontalAndVerticalConstraints;
    Constraint* constraints = new Constraint[totalConstraints];
    QMap<QString, int> identifierToSolvePointsIndex;

    double _parameters[100];
    SketchSolvePoint _points[100];
    Line _lines[100];
    Constraint _constraints[100];
    QMap<QString, int> _idToIndex;

    // ----------------------------------------------------------
    // new implementation for points
    int i = 0;
    foreach(QSharedPointer<ConstrainedPoint> point, this->points) {
        _points[i].x = &_parameters[2*i];
        _points[i].y = &_parameters[2*i + 1];

        _parameters[2*i] = point->x()->value();
        _parameters[2*i + 1] = point->y()->value();

        _idToIndex[point->identifier()] = i;

        i = i + 1;
    }

    int parametersIndex = 2*i;

    QSet<int> _identifiersSeen;

    i = 0;
    int cIndex = 0;
    // new implementations for lines and constraints
    foreach(QSharedPointer<ConstrainedLine> line, this->lines) {
        if(!_identifiersSeen.contains(line->identifier)) {
                _identifiersSeen += line->identifier;


            _lines[i].p1 = _points[_idToIndex[line->start->identifier()]];
            _lines[i].p2 = _points[_idToIndex[line->end->identifier()]];

            if(line->isDistanceFixed()) {
                _constraints[cIndex].line1 = _lines[i];
                _constraints[cIndex].type = lineLength;
                _constraints[cIndex].parameter = &_parameters[parametersIndex];
                _parameters[parametersIndex] = line->getDesiredDistance() * pixelPerMmScale;

                cIndex++;
                parametersIndex++;
            }

            if(line->isHorizontalConstrained()) {
                _constraints[cIndex].line1 = _lines[i];
                _constraints[cIndex].type = sketchSolveHorizontal;

                cIndex++;
            }

            if(line->isVerticallyConstrained()) {
                _constraints[cIndex].line1 = _lines[i];
                _constraints[cIndex].type = sketchSolveVertical;

                cIndex++;
            }

            i++;
        }
    }

    double *_pparameters[100];
    for(int i=0;i<100;i++) {
            _pparameters[i] = &_parameters[i];
    }

    int _result = ::solve(_pparameters, parametersIndex, _constraints, cIndex, fine);


    // ----------------------------------------------------------

    i = 0;
    foreach(QSharedPointer<ConstrainedPoint> point, this->points) {
        if(!point->fixedX()) {
            parameters.append(point->x().data()->address());
        }
        if(!point->fixedY()) {
            parameters.append(point->y().data()->address());
        }

        solvePoints[i].x = point->x().data()->address().data();
        solvePoints[i].y = point->y().data()->address().data();

        identifierToSolvePointsIndex.insert(point->identifier(), i);

        i++;
    }

    foreach(QSharedPointer<double> p, parameters) {
        qDebug() << p << ":" << *(p.data()) << p.data();
    }

    double* pparameters[parameters.size()];
    QSet<int> identifiersSeen;
    QList<double> lengthParameters;

    i = 0;
    int constraintIndex = 0;
    foreach(QSharedPointer<ConstrainedLine> line, this->lines) {
        if(!identifiersSeen.contains(line->identifier)) {
            identifiersSeen += line->identifier;

            solveLines[i].p1 = solvePoints[identifierToSolvePointsIndex[line->start->identifier()]];
            solveLines[i].p2 = solvePoints[identifierToSolvePointsIndex[line->end->identifier()]];

            if(line->isDistanceFixed()) {
                constraints[constraintIndex].line1 = solveLines[i];
                lengthParameters += line->getDesiredDistance() * pixelPerMmScale;
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
    foreach(QSharedPointer<double> parameter, parameters) {
        pparameters[i] = parameter.data();

        i++;
    }


#ifdef CARPENTER_DEBUG
    qDebug() << "SketchSolver: line constraints";
    qDebug() << "SketchSolver: ----------------\n";
    qDebug() << "SketchSolver: constraints: " << this->constraints.size();
    qDebug() << "SketchSolver: parameters: " << parameters.size();

    qDebug() << "SketchSolver: solver";
    qDebug() << "SketchSolver: ------";
    qDebug() << "SketchSolver: constraintsIndex:" << constraintIndex;
    qDebug() << "SketchSolver: total constraints:" << totalConstraints;
#endif


    //int result = ::solve(pparameters, parameters.size(), constraints, totalConstraints, fine);
    int result = _result;
    //int result = noSolution;

    if(result == succsess) {
#ifdef CARPENTER_DEBUG
        qDebug() << "SketchSolver: solution found";
        qDebug() << "SketchSolver: visited edges" << visitedEdge;
        qDebug() << "SketchSolver: applied constraints" << appliedConstraints;
#endif

        foreach(QSharedPointer<ConstrainedPoint> point, this->points) {
#ifdef CARPENTER_DEBUG
            qDebug() << "SketchSolver: (" << point->x().data()->value() << "," << point->y().data()->value() << ")";
            qDebug() << "SketchSolver: " << point->identifier();
#endif
        }

        i = 0;
        foreach(QSharedPointer<ConstrainedPoint> point, this->points) {
            int id = _idToIndex[point->identifier()];
            *(point->x()->address()) = *_points[id].x;
            *(point->y()->address()) = *_points[id].y;

            i++;
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
#ifdef CARPENTER_DEBUG
        qDebug() << "SketchSolver: the sketch wasn't solved";
#endif
        return;
    }

    QVariantMap map;

    foreach(QSharedPointer<ConstrainedPoint> point, this->points) {
        map.insert(point->identifier(), QVector2D(point->x().data()->value(), point->y().data()->value()));
    }

#ifdef CARPENTER_DEBUG
    qDebug() << "SketchSolver: new points per identifier " << map;
#endif

    QVariant returnedValue;
    QMetaObject::invokeMethod(this->sketch, "movePoints",
            Q_ARG(QVariant, QVariant::fromValue(map)));
}
