#include "constrainedpoint.h"

#include <QVector2D>
#include <QDebug>


ConstrainedPoint::ConstrainedPoint(double x, double y, QObject* point)
{
    this->relatedPoint = point;
    this->x = QSharedPointer<Parameter>(new Parameter(x));

    this->y = QSharedPointer<Parameter>(new Parameter(y));

    this->fixedX = false;
    this->fixedY = false;
    this->constraintAmount = 0;
}

bool ConstrainedPoint::tryToSetX(QSharedPointer<Parameter> x) {
    if(this->fixedX) {
        // already fixed, need to replace argument x's pointer
        (*(x)).setValue((*(this->x)).address());
    }
    else {
        this->fixedX = true;
    }
    this->x = x;

    return true;
}

bool ConstrainedPoint::tryToSetY(QSharedPointer<Parameter> y) {
    if(this->fixedY) {
        // already fixed, need to replace argument y's pointer
        (*(y)).setValue((*(this->y)).address());
    }
    else {
        this->fixedY = true;
    }

    this->y = y;
    return true;
}

QVector2D ConstrainedPoint::point() {
    return QVector2D((*(this->x)).value(),(*(this->y)).value());
}

ConstrainedPoint::operator QString() const {
    return "Point";
}

QString ConstrainedPoint::identifier() {
    return this->relatedPoint->property("identifier").toString();
}

void ConstrainedPoint::increaseConstraintAmount() {
    this->constraintAmount++;
}

int ConstrainedPoint::getConstraintAmount() {
    return this->constraintAmount;
}
