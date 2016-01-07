#ifndef CONSTRAINEDPOINT_H
#define CONSTRAINEDPOINT_H

#include <QObject>
#include <QVector2D>
#include <QVariant>
#include <QSharedPointer>
#include "parameter.h"

/**
 * \class ConstrainedPoint
 * @brief Represents a point that can be constrained
 *
 * Class that represents a point component used by a constraint solver
 */
class ConstrainedPoint
{
    public:
        ConstrainedPoint(double x, double y, QObject* point);

        /**
         * @brief setX
         * @param x New parameter of the coordinate x
         *
         * The x coordinate will be replaced by the provided one. If the
         * x coordinate is already constrained, the provided parameter
         * will be replaced with the current used
         */
        void setX(QSharedPointer<Parameter> x);
        /**
         * @brief setY
         * @param y New parameter of the coordinate y
         *
         * The y coordinate will be replaced by the provided one. If the
         * y coordinate is already constrained, the provided parameter
         * will be replaced with the current used
         */
        void setY(QSharedPointer<Parameter> y);

        QSharedPointer<Parameter> x();
        QSharedPointer<Parameter> y();

        /**
         * @brief fixedX
         * @return True iff the x coordinate is constrained
         */
        bool fixedX();
        /**
         * @brief fixedX
         * @return True iff the y coordinate is constrained
         */
        bool fixedY();

        QString identifier();
    private:
        QObject* relatedPoint;

        QSharedPointer<Parameter> _x;
        QSharedPointer<Parameter> _y;

        bool _fixedX;
        bool _fixedY;
};

#endif // CONSTRAINEDPOINT_H
