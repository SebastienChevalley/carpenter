#ifndef CONSTRAINEDLINE_H
#define CONSTRAINEDLINE_H

#include "constrainedpoint.h"

class ConstrainedLine
{
    public:
        ConstrainedLine(ConstrainedPoint* start, ConstrainedPoint* end, int identifier);
        ConstrainedLine* verticallyConstrained();
        ConstrainedLine* horizontallyConstrained();
        operator QString() const;
        ConstrainedPoint* start;
        ConstrainedPoint* end;
        bool isVerticallyConstrained();
        bool isHorizontalConstrained();
        bool isDistanceFixed();
        double getDesiredDistance();
        ConstrainedLine* setDesiredDistance(double distance);
        double desiredDistance;
        int identifier;


    private:
        bool vertical;
        bool horizontal;
        bool distanceFixed;

};

#endif // CONSTRAINEDLINE_H
