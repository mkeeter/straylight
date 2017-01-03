#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>

#include "bridge.hpp"
#include "canvas.hpp"

////////////////////////////////////////////////////////////////////////////////


int main(int argc, char**argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Register canvas class for drawing
    QQmlApplicationEngine engine;
    qmlRegisterType<CanvasObject>("FbItem", 1, 0, "FbItem");

    // Install Bridge singleton
    qmlRegisterSingletonType<Bridge>("Bridge", 1, 0, "Bridge", Bridge::singleton);

    // Register pure-QML singletons
    qmlRegisterSingletonType(QUrl("qrc:/qml/Material.qml"), "Material", 1, 0, "Material");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Style.qml"), "Style", 1, 0, "Style");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");

    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();

    return 1;
}
