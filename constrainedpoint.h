#ifndef CONSTRAINEDPOINT_H
#define CONSTRAINEDPOINT_H

#include <QObject>
#include <QVector2D>
#include <QVariant>
#include <QSharedPointer>

class ConstrainedPoint
{
    public:
        ConstrainedPoint(double x, double y, QObject* point);
        bool tryToSetX(QSharedPointer<double> x);
        bool tryToSetY(QSharedPointer<double> y);
        QVector2D point();
        operator QString() const;
        QSharedPointer<double> x;
        QSharedPointer<double> y;
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
