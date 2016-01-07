#include "constrainedpoint.h"

#include <QVector2D>
#include <QDebug>


ConstrainedPoint::ConstrainedPoint(double x, double y, QObject* point)
{
    this->relatedPoint = point;
    this->_x = QSharedPointer<Parameter>(new Parameter(x));

    this->_y = QSharedPointer<Parameter>(new Parameter(y));

    this->_fixedX = false;
    this->_fixedY = false;
}

void ConstrainedPoint::setX(QSharedPointer<Parameter> x) {
    if(this->_fixedX) {
        // already fixed, need to replace argument x's pointer
        (*(x)).setValue((*(this->_x)).address());
    }
    else {
        this->_fixedX = true;
    }
    this->_x = x;
}

void ConstrainedPoint::setY(QSharedPointer<Parameter> y) {
    if(this->_fixedY) {
        // already fixed, need to replace argument y's pointer
        (*(y)).setValue((*(this->_y)).address());
    }
    else {
        this->_fixedY = true;
    }

    this->_y = y;
}

QString ConstrainedPoint::identifier() {
    return this->relatedPoint->property("identifier").toString();
}

QSharedPointer<Parameter> ConstrainedPoint::x() {
    return this->_x;
}

QSharedPointer<Parameter> ConstrainedPoint::y() {
    return this->_y;
}

bool ConstrainedPoint::fixedX() {
    return this->_fixedX;
}

bool ConstrainedPoint::fixedY() {
    return this->_fixedY;
}

