#include <QDebug>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>

#include "bridge.hpp"
#include "material.hpp"
#include "canvas.hpp"

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char**argv)
{
    {   // Configure default OpenGL as 3.3 Core
        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);
    }

    // Register canvas class for drawing
    qmlRegisterType<CanvasObject>("Canvas", 1, 0, "Canvas");

    // Install Bridge singleton
    qmlRegisterSingletonType<Bridge>("Bridge", 1, 0, "Bridge", Bridge::singleton);
    qmlRegisterSingletonType<MaterialSingleton>(
            "Material", 1, 0, "Material", MaterialSingleton::singleton);

    // Register pure-QML singletons
    qmlRegisterSingletonType(QUrl("qrc:/qml/Material.qml"), "Material", 1, 0, "Material");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Style.qml"), "Style", 1, 0, "Style");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}
