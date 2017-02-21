#include <QDebug>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>

#include "app/bridge/bridge.hpp"
#include "app/bridge/graph.hpp"
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

    //  Install Bridge singleton to make it available in QML
    qmlRegisterSingletonType<App::Bridge::Bridge>(
            "Bridge", 1, 0, "Bridge", App::Bridge::Bridge::instance);
    qmlRegisterSingletonType<App::Bridge::GraphModel>(
            "Graph", 1, 0, "Graph", App::Bridge::GraphModel::instance);

    // Construct bridge singleton
    App::Bridge::Bridge::instance();

    // Install Material singleton (available from both C++ / QML)
    qmlRegisterSingletonType<App::UI::Material>(
            "Material", 1, 0, "Material", App::UI::Material::singleton);

    // Register pure-QML singletons
    qmlRegisterSingletonType(QUrl("qrc:/qml/Style.qml"), "Style", 1, 0, "Style");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");

    QGuiApplication app(argc, argv);

    // TODO: why does this need to be heap-allocated?
    auto engine = new QQmlApplicationEngine;
    engine->load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}
