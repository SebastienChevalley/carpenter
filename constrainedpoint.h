#ifndef CONSTRAINEDPOINT_H
#define CONSTRAINEDPOINT_H

#include <QObject>
#include <QVector2D>
#include <QVariant>
#include <QSharedPointer>

class ConstrainedPoint
{
    public:
        ConstrainedPoint(float x, float y, QObject* point);
        bool tryToSetX(QSharedPointer<float> x);
        bool tryToSetY(QSharedPointer<float> y);
        QVector2D point();
        operator QString() const;
        QSharedPointer<float> x;
        QSharedPointer<float> y;
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
