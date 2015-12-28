#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include <sketch_converter.h>
#include <sketchconstraintssolver.h>
#include <testnewtonsolver.h>
#include <testtriangulate.h>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<SketchConverter>("SketchConverter", 1, 0, "SketchConverter");
    qmlRegisterType<SketchConstraintsSolver>("SketchConstraintsSolver", 1, 0, "SketchConstraintsSolver");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    //SketchConverter *pouet = new SketchConverter();

    //testnewtonsolver test;
    //TestTriangulate test2;

    return app.exec();
}

