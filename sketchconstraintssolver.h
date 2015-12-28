#ifndef SKETCHCONSTRAINTSSOLVER_H
#define SKETCHCONSTRAINTSSOLVER_H

#include "constrainedpoint.h"
#include "constrainedline.h"
#include <QObject>
#include <QVector2D>
#include <QList>
#include <QHash>
#include <QVariantMap>
#include <QVariantList>
#include <QSet>
#include <QStack>
#include <QQueue>
#include <QMetaObject>
#include <functional>

class SketchConstraintsSolver : public QObject
{
    Q_OBJECT
    public:
        explicit SketchConstraintsSolver(QObject *parent = 0);

    public slots:
        bool solve();
        void setSketch(QObject *sketch);
        void applyOnSketch();

    private:
        bool solved;
        QObject *sketch;
        QVector2D origin;
        QHash<int, ConstrainedPoint*> points;
        QList<ConstrainedLine*> lines;
        QList<ConstrainedLine*> constraints;
};

#endif // SKETCHCONSTRAINTSSOLVER_H
