#include "testnewtonsolver.h"
#include "solve.h"
#include <QDebug>
#include <QSharedPointer>

testnewtonsolver::testnewtonsolver()
{
    double x0 = 8;
    double x1 = 7;

    double EPSILON = 0.001;
    double MAX_STEP = 50;

    auto df11 = [](double x1, double x2) {
        return 2 * x1;
    };
    auto df12 = [](double x1, double x2) {
        return 0;
    };
    auto df21 = [](double x1, double x2) {
        return 2 * (x1 - x2);
    };
    auto df22 = [](double x1, double x2) {
        return - 2 * (x1 - x2);
    };

    auto f1 = [](double x1, double x2) {
        return x1 * x1 - 100;
    };

    auto f2 = [](double x1, double x2) {
        return (x1 - x2)*(x1 - x2);
    };


    qDebug() << "start";
    qDebug() << "-----";

    qDebug() << "x0 =" << x0 << "; x1 =" << x1;

    double stepCount = 0;

    double oldX0;
    double oldX1;

    Mat jacobian = Mat(2,2, CV_64F);
    Mat oldXs = Mat(2,1, CV_64F);
    Mat fX = Mat(2,1, CV_64F);
    Mat newXs = Mat(2,1, CV_64F);

    Mat alternateXs = Mat(2,1, CV_64F);
    /*
    do {
        oldX0 = x0;
        oldX1 = x1;

        /*jacobian.at<double>(0,0) = df11(oldX0, oldX1);
        jacobian.at<double>(0,1) = df12(oldX0, oldX1);
        jacobian.at<double>(1,0) = df21(oldX0, oldX1);
        jacobian.at<double>(1,1) = df22(oldX0, oldX1);

        oldXs.at<double>(0,0) = oldX0;
        oldXs.at<double>(1,0) = oldX1;

        //fX.at<double>(0,0) = f1(oldX0, oldX1);
        //fX.at<double>(1,0) = f2(oldX0, oldX1);

        alternateXs.at<double>(0,0) = (f1(oldX0, oldX1) + f2(oldX0, oldX1)) / (df11(oldX0, oldX1) + df21(oldX0, oldX1));
        alternateXs.at<double>(1,0) = (f1(oldX0, oldX1) + f2(oldX0, oldX1)) / (df12(oldX0, oldX1) + df22(oldX0, oldX1));

        oldXs.at<double>(0,0) = oldX0;
        oldXs.at<double>(1,0) = oldX1;

        //qDebug() << "jacobian" << jacobian;
        //qDebug() << "oldXs" <<oldXs;
        //qDebug() << "fX" << fX;

        //newXs= oldXs - jacobian.inv() * fX;
        newXs = oldXs - alternateXs;

        x0 = newXs.at<double>(0,0);
        x1 = newXs.at<double>(1,0);

        //qDebug() << "oldX0 =" << oldX0 << "; oldX1 =" << oldX1;
        qDebug() << "x0 =" << x0 << "; x1 =" << x1;

        //qDebug() << "delta x0 =" << oldX0 - x0 << "; delta x1 =" << oldX1- x1;

        stepCount++;

        //qDebug() << "cond1" << (stepCount < MAX_STEP);
        //qDebug() << "cond2" << ((oldX0 - x0) > EPSILON);
        //qDebug() << "cond3" << ((oldX1- x1) > EPSILON);

    } while(stepCount < MAX_STEP && (abs(oldX0 - x0) > EPSILON || abs(oldX1- x1) > EPSILON));*/

    /*
     * using sketchsolve
     */
    SketchSolvePoint points[7];
    Line lines[8];
    Constraint constraints[9];
    double parameters[14];

    parameters[0] = -1.78;
    parameters[1] = 1.76;

    parameters[2] = -0.26;
    parameters[3] = 1.72;

    parameters[4] = -0.22;
    parameters[5] = 0.3;

    parameters[6] = -1.72;
    parameters[7] = 0.48;

    parameters[8] = 4.72;
    parameters[9] = 0.58;

    parameters[10] = 0.22;
    parameters[11] = -1.7;

    parameters[12] = 4.66;
    parameters[13] = -1.64;


    points[0].x = &parameters[0];
    points[0].y = &parameters[1];

    points[1].x = &parameters[2];
    points[1].y = &parameters[3];

    points[2].x = &parameters[4];
    points[2].y = &parameters[5];

    points[3].x = &parameters[6];
    points[3].y = &parameters[7];

    points[4].x = &parameters[8];
    points[4].y = &parameters[9];

    points[5].x = &parameters[10];
    points[5].y = &parameters[11];

    points[6].x = &parameters[12];
    points[6].y = &parameters[13];

    lines[0].p1 = points[0];
    lines[0].p2 = points[1];

    lines[1].p1 = points[1];
    lines[1].p2 = points[2];

    lines[2].p1 = points[2];
    lines[2].p2 = points[3];

    lines[3].p1 = points[3];
    lines[3].p2 = points[0];

    lines[4].p1 = points[2];
    lines[4].p2 = points[4];

    lines[5].p1 = points[4];
    lines[5].p2 = points[6];

    lines[6].p1 = points[6];
    lines[6].p2 = points[5];

    lines[7].p1 = points[2];
    lines[7].p2 = points[5];

    constraints[0].line1 = lines[0];
    constraints[1].line1 = lines[1];
    constraints[2].line1 = lines[2];
    constraints[3].line1 = lines[3];
    constraints[4].line1 = lines[4];
    constraints[5].line1 = lines[5];
    constraints[6].line1 = lines[6];
    constraints[7].line1 = lines[7];

    constraints[0].type = sketchSolveHorizontal;
    constraints[1].type = sketchSolveVertical;
    constraints[2].type = sketchSolveHorizontal;
    constraints[3].type = sketchSolveVertical;

    constraints[4].type = sketchSolveHorizontal;
    constraints[5].type = sketchSolveVertical;
    constraints[6].type = sketchSolveHorizontal;
    constraints[7].type = sketchSolveVertical;

    constraints[7].line1 = lines[5];
    constraints[7].type = lineLength;

    double honk = 20;
    constraints[7].parameter = &honk;

    double *pparameters[14];
    for(int i = 0; i < 14; i++) {
        pparameters[i] = &parameters[i];
    }

    QSharedPointer<double> hola = QSharedPointer<double>(new double);

    *(hola) = 212.0;

    double* ciao = hola.data();

    *ciao = 20;

    qDebug() << "hola" << *hola;


    int result = solve(pparameters, 14, constraints, 8, fine);

    qDebug() << "resultat";
    qDebug() << result;

    if(result == succsess) {
        qDebug() << "solution found";

        for(int i = 0; i < 7; i++) {
            qDebug() << "" << i << "(" << *pparameters[i*2] << "," <<  *pparameters[i*2+1] << ")";
        }
    }
    else if(result == noSolution) {
        qDebug() << "solution not found";
    }

}
