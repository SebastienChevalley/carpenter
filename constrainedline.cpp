#include "constrainedline.h"
#include <QDebug>

ConstrainedLine::ConstrainedLine(QSharedPointer<ConstrainedPoint> start,
                                 QSharedPointer<ConstrainedPoint> end,
                                 int identifier)
{
    this->identifier = identifier;
    this->horizontal = false;
    this->vertical = false;
    this->start = start;
    this->end = end;
    this->distanceFixed = false;
}

ConstrainedLine* ConstrainedLine::horizontallyConstrained() {
#ifdef CARPENTER_DEBUG
    qDebug() << "ConstrainedLine: horizontal constrained";
#endif
    this->horizontal = true;
    return this;
}

ConstrainedLine* ConstrainedLine::verticallyConstrained() {
#ifdef CARPENTER_DEBUG
    qDebug() << "ConstrainedLine: vertical constrained";
#endif
    this->vertical = true;
    return this;
}

bool ConstrainedLine::isVerticallyConstrained() {
    return this->vertical;
}

bool ConstrainedLine::isHorizontalConstrained() {
    return this->horizontal;
}

bool ConstrainedLine::isDistanceFixed() {
    return this->distanceFixed;
}

ConstrainedLine* ConstrainedLine::setDesiredDistance(double distance) {
    this->distanceFixed = true;
    this->desiredDistance = distance;
    return this;
}

double ConstrainedLine::getDesiredDistance() {
    return this->desiredDistance;
}
