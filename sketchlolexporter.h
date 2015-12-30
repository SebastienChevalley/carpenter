#ifndef SKETCHLOLEXPORTER_H
#define SKETCHLOLEXPORTER_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include <QVector2D>

class SketchLolExporter : public QObject {
    Q_OBJECT

    Q_PROPERTY(QObject* sketch READ getSketch WRITE setSketch)

    public:
        explicit SketchLolExporter(QObject *parent = 0);

    public slots:
        void setSketch(QObject *sketch);
        QObject* getSketch();
        QVariant exportToFile(QString path);

    private:
        QObject *sketch;
        QString idToLetter[26];
        bool identifierToLetter(int id, QString &name);
};

#endif // SKETCHLOLEXPORTER_H
