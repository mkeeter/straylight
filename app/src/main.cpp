#include <QDebug>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>

#include "core/bridge.hpp"
#include "core/undo.hpp"
#include "ui/material.hpp"
#include "render/canvas.hpp"

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

    // Install Bridge singleton
    qmlRegisterSingletonType<App::Core::Bridge>(
            "Bridge", 1, 0, "Bridge", App::Core::Bridge::singleton);
    qmlRegisterSingletonType<App::Core::UndoStack>(
            "UndoStack", 1, 0, "UndoStack", App::Core::UndoStack::singleton);
    qmlRegisterSingletonType<App::UI::Material>(
            "Material", 1, 0, "Material", App::UI::Material::singleton);

    // Register pure-QML singletons
    qmlRegisterSingletonType(QUrl("qrc:/qml/Style.qml"), "Style", 1, 0, "Style");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));

    // Construct a default cell
    App::Core::Bridge::root()->insertCell(0, "x", "0");
    App::Core::Bridge::root()->insertCell(0, "y", "(+ 1 0)");

    return app.exec();
}
