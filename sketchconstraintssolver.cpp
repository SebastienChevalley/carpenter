#include "sketchconstraintssolver.h"

#include <QDebug>
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
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

        ConstrainedLine* cLine1 = new ConstrainedLine(this->points[start], this->points[end]);
        ConstrainedLine* cLine2 = new ConstrainedLine(this->points[end], this->points[start]);

        this->points[start]->increaseConstraintAmount();
        this->points[end]->increaseConstraintAmount();

        if(distanceFixed) {
            float desiredDistance = line->property("desiredDistance").toFloat();

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

    // apply distance constraints

    int freeVariables = 0;
    int equationsCounts = this->constraints.size();

    if(equationsCounts > 0) {
        // should traverse all the graphs to remove the unneeded free variables

        if(freeVariables > equationsCounts) {
            // need to fix some points, to much degrees of freedom
            // should classify point by their freedomness


        }

        if(freeVariables < equationsCounts) {
            qDebug() << "too much constraints for the number of free variables";
            return false;
        }

        Mat jacobian = Mat(equationsCounts,equationsCounts, CV_64F);
        Mat oldXs = Mat(equationsCounts,1, CV_64F);
        Mat fX = Mat(equationsCounts,1, CV_64F);
        Mat newXs = Mat(equationsCounts,1, CV_64F);

        /*do {
            for(int i = 0; i < equationsCounts; i++) {
                jacobian.at<float>(i, constraints[i]->i1) = constraints[i]->df1(
                            oldXs[constraints[i]->i1],
                            oldXs[constraints[i]->i2],
                            oldXs[constraints[i]->i3],
                            oldXs[constraints[i]->i4],
                );

                oldXs.at<double>(i,0) = oldXs.at(i, 0);
            }

            oldXs.at<double>(0,0) = oldX0;
            oldXs.at<double>(1,0) = oldX1;

            fX.at<double>(0,0) = f1(oldX0, oldX1);
            fX.at<double>(1,0) = f2(oldX0, oldX1);


        } while(stepCount < STEPS_COUNT && enoughPrecision(oldXs, newXs, equationsCounts));*/

    }

    qDebug() << "visited edges" << visitedEdge;
    qDebug() << "applied constraints" << appliedConstraints;

    foreach(ConstrainedPoint* point, this->points) {
        qDebug() << "(" << *(point->x.data()) << "," << *(point->y.data()) << ")";
        qDebug() << point->identifier();
    }

    this->solved = true;
    return this->solved;
}

std::function<double(double, double, double, double)> SketchConstraintsSolver::partialDerivativeFunction(
        ConstrainedPoint* p1,
        ConstrainedPoint* p2,
        int variable
) {
    /*
     * Variables :
     * p1 = (x1,y1)
     * p2 = (x2,y2)
     *
     * 1 : x1
     * 2 : y1
     * 3 : x2
     * 4 : y2
     *
     *
     * The formula is
     * f: sqrt((x1-x2)² - (y1-y2)²) = d
     * => f: (x1-x2)² - (y1-y2)² - d² = 0
     * => df/dx1 = 2*(x1-x2)
     * => df/dy1 = 2*(y1-y2)
     * => df/dx2 = -2*(x1-x2)
     * => df/dy2 = -2(y1-y2)
     *
     * Take care if the variable is fixed, otherwise the formula is 0
     */

    float minus = (variable == 3 || variable == 4) ? -1 : 1;

    float zero = 1;
    if(
        (variable == 1 && p1->fixedX)
        || (variable == 2 && p1->fixedY)
        || (variable == 3 && p2->fixedX)
        || (variable == 4 && p2->fixedY)
    ) {
        zero = 0;
    }

    float takeXs = 0;
    float takeYs = 0;

    if(variable == 1 || variable == 3) {
        takeXs = 1;
    }
    if(variable == 2 || variable == 4) {
        takeYs = 1;
    }

    return [=](double x1, double y1, double x2, double y2) {
        return zero * minus * 2 * ( takeXs * (x1-x2) + takeYs * (y1-y2));
    };
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
