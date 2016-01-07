#ifndef CONSTRAINEDLINE_H
#define CONSTRAINEDLINE_H


#include <QSharedPointer>

#include "constrainedpoint.h"

/**
 * \class ConstrainedLine
 * @brief Represents a line that can be constrained
 *
 * Class that represents a line component used by a constraint solver
 */
class ConstrainedLine
{
    public:
        ConstrainedLine(QSharedPointer<ConstrainedPoint> start,
                        QSharedPointer<ConstrainedPoint> end,
                        int identifier);
        /**
         * @brief verticallyConstrained Constrains the line vertically
         * @return The same line but vertically constrained
         */
        ConstrainedLine* verticallyConstrained();
        /**
         * @brief horizontallyConstrained Constrains the line horizontally
         * @return The same line but horizontally constrained
         */
        ConstrainedLine* horizontallyConstrained();

        /**
         * Endpoints of the line
         */
        QSharedPointer<ConstrainedPoint> start;
        QSharedPointer<ConstrainedPoint> end;

        bool isVerticallyConstrained();
        bool isHorizontalConstrained();

        bool isDistanceFixed();
        double getDesiredDistance();
        ConstrainedLine* setDesiredDistance(double distance);


        int identifier;
    private:
        bool vertical;
        bool horizontal;
        bool distanceFixed;
        double desiredDistance;

};

#endif // CONSTRAINEDLINE_H
