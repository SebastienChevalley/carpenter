#ifndef CONSTRAINEDLINE_H
#define CONSTRAINEDLINE_H

#include "constrainedpoint.h"

class ConstrainedLine
{
    public:
        ConstrainedLine(ConstrainedPoint* start, ConstrainedPoint* end);
        ConstrainedLine* verticallyConstrained();
        ConstrainedLine* horizontallyConstrained();
        operator QString() const;
        ConstrainedPoint* start;
        ConstrainedPoint* end;
        bool isVerticallyConstrained();
        bool isHorizontalConstrained();
        bool isDistanceFixed();
        float getDesiredDistance();
        ConstrainedLine* setDesiredDistance(float distance);

    private:
        bool vertical;
        bool horizontal;
        float desiredDistance;
        bool distanceFixed;

};

#endif // CONSTRAINEDLINE_H
