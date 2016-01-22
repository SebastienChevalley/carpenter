#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include <sketch_converter.h>
#include <sketchconstraintssolver.h>
#include <sketchlolexporter.h>
#include <displaykeyboard.h>

#include <QSharedPointer>
#include <QSharedDataPointer>
#include <QDir>
#include <QDebug>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<SketchConverter>("SketchConverter", 1, 0, "SketchConverter");
    qmlRegisterType<SketchConstraintsSolver>("SketchConstraintsSolver", 1, 0, "SketchConstraintsSolver");
    qmlRegisterType<SketchLolExporter>("SketchLolExporter", 1, 0, "SketchLolExporter");
    qmlRegisterType<DisplayKeyboard>("DisplayKeyboard", 1, 0, "DisplayKeyboard");

    engine.rootContext()->setContextProperty("appPath", QDir().currentPath());
    engine.rootContext()->setContextProperty("appUrl", QUrl::fromLocalFile(QDir().currentPath()).toString());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

