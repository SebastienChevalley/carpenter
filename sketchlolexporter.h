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

/**
 * @brief The SketchLolExporter class
 *
 * SketchLolExporter allows to export the current
 * sketch document in a flat text file.
 *
 * After qml registration, it can be used as a qml
 * component like that :
 * SketchLolExporter { sketch: mySketchComponent },
 */
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
