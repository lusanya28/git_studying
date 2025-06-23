#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "temperaturemodel.h"
#include "temperaturecontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Create model and controller
    TemperatureModel* model = new TemperatureModel();
    TemperatureController* controller = new TemperatureController(model);

    QQmlApplicationEngine engine;

    // Handle object creation failure
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Register model and controller with QML
    engine.rootContext()->setContextProperty("model", model);
    engine.rootContext()->setContextProperty("controller", controller);

    // Load QML
    engine.loadFromModule("temperature_converter", "Main");

    return app.exec();
}
