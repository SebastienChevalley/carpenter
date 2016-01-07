#ifndef SKETCHCONSTRAINTSSOLVER_H
#define SKETCHCONSTRAINTSSOLVER_H


#include <QObject>
#include <QVector2D>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QSet>
#include <QStack>
#include <QQueue>
#include <QMetaObject>
#include <QSharedPointer>

#include "constrainedpoint.h"
#include "constrainedline.h"
#include "parameter.h"

/**
 * @brief The SketchConstraintsSolver class
 *
 * Constrain solver module that can solve
 * vertical, horizontal and line length constraints
 *
 * can be used in a QML document by instantiate it (after
 * qml registration)
 *
 * SketchConstraintsSolver { sketch: mySketchComponent },
 * you can call solve on it to find a solution and applyOnSketch
 * to apply it if the solver succeeds
 */
class SketchConstraintsSolver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* sketch READ getSketch WRITE setSketch)

    public:
        explicit SketchConstraintsSolver(QObject *parent = 0);

    public slots:
        /**
         * Try to solve the Sketch's constraints
         *
         * If the solving was successful, developper need to call
         * applyOnSketch method to apply the solution found on the
         * Sketch.
         *
         * @brief solve
         * @return boolean true if a solution was found, QString
         * with error message otherwise (needed for Javascript side)
         */
        QVariant solve();

        /**
         * Apply the solver solution on the sketck. Works only
         * if a solution was found with the solve method.
         *
         * @brief applyOnSketch
         */
        void applyOnSketch();
        void setSketch(QObject *sketch);
        QObject* getSketch();

    private:
        bool solved;
        QObject* sketch;
        QVector2D origin;
        QHash<int, QSharedPointer<ConstrainedPoint>> points;
        QList<QSharedPointer<ConstrainedLine>> lines;
        QList<QSharedPointer<ConstrainedLine>> constraints;
};

#endif // SKETCHCONSTRAINTSSOLVER_H
