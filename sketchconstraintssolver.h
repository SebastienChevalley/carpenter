#ifndef SKETCHCONSTRAINTSSOLVER_H
#define SKETCHCONSTRAINTSSOLVER_H

#include "constrainedpoint.h"
#include "constrainedline.h"
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
#include <functional>
#include "parameter.h"

class SketchConstraintsSolver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* sketch READ getSketch WRITE setSketch)

    public:
        explicit SketchConstraintsSolver(QObject *parent = 0);

    public slots:
        QVariant solve();
        void applyOnSketch();
        void setSketch(QObject *sketch);
        QObject* getSketch();

    private:
        bool solved;
        QObject* sketch;
        QVector2D origin;
        QHash<int, ConstrainedPoint*> points;
        QList<ConstrainedLine*> lines;
        QList<ConstrainedLine*> constraints;
};

#endif // SKETCHCONSTRAINTSSOLVER_H
