#ifndef CONSTRAINEDPOINT_H
#define CONSTRAINEDPOINT_H

#include <QObject>
#include <QVector2D>
#include <QVariant>
#include <QSharedPointer>
#include "parameter.h"


class ConstrainedPoint
{
    public:
        ConstrainedPoint(double x, double y, QObject* point);
        bool tryToSetX(QSharedPointer<Parameter> x);
        bool tryToSetY(QSharedPointer<Parameter> y);
        QVector2D point();
        operator QString() const;
        QSharedPointer<Parameter> x;
        QSharedPointer<Parameter> y;
        QString identifier();
        bool fixedX;
        bool fixedY;
        void increaseConstraintAmount();
        int getConstraintAmount();

    private:
        QObject* relatedPoint;
        int constraintAmount;

};

#endif // CONSTRAINEDPOINT_H
