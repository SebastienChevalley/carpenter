#include "constrainedpoint.h"

#include <QVector2D>
#include <QDebug>


ConstrainedPoint::ConstrainedPoint(float x, float y, QObject* point)
{
    this->relatedPoint = point;
    this->x = QSharedPointer<float>(new float);
    *(this->x) = x;

    this->y = QSharedPointer<float>(new float);
    *(this->y) = y;

    this->fixedX = false;
    this->fixedY = false;
    this->constraintAmount = 0;
}

bool ConstrainedPoint::tryToSetX(QSharedPointer<float> x) {
    if(this->fixedX) {
        return false;
    }
    else {
        this->x = x;
        this->fixedX = true;
        return true;
    }
}

bool ConstrainedPoint::tryToSetY(QSharedPointer<float> y) {
    if(this->fixedY) {
        return false;
    }
    else {
        this->y = y;
        this->fixedY = true;
        return true;
    }
}

QVector2D ConstrainedPoint::point() {
    return QVector2D(*(this->x),*(this->y));
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
