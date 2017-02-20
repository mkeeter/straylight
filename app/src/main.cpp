#include <QDebug>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>

#include "app/bridge/bridge.hpp"
#include "app/ui/material.hpp"
#include "app/render/canvas.hpp"

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
    qmlRegisterType<App::Render::CanvasObject>("Canvas", 1, 0, "Canvas");

    // Install Material singleton (available from both C++ / QML)
    qmlRegisterSingletonType<App::UI::Material>(
            "Material", 1, 0, "Material", App::UI::Material::singleton);

    // Register pure-QML singletons
    qmlRegisterSingletonType(QUrl("qrc:/qml/Style.qml"), "Style", 1, 0, "Style");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}
