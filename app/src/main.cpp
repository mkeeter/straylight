#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickItem>
#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObject>

#include "bridge.hpp"

////////////////////////////////////////////////////////////////////////////////

class FbItemRenderer : public QQuickFramebufferObject::Renderer
{
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size)
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        // optionally enable multisampling by doing format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    void render()
    {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        // Do rendering here
    }
};

class FbItem : public QQuickFramebufferObject
{
    QQuickFramebufferObject::Renderer *createRenderer() const override
    {
        return new FbItemRenderer;
    }
};

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char**argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<FbItem>("FbItem", 1, 0, "FbItem");
    qmlRegisterSingletonType<Bridge>("Bridge", 1, 0, "Bridge", Bridge::singleton);

    qmlRegisterSingletonType(QUrl("qrc:/qml/Colors.qml"), "Colors", 1, 0, "Colors");
    qmlRegisterSingletonType(QUrl("qrc:/qml/Awesome.qml"), "Awesome", 4, 7, "Awesome");
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();

    return 1;
}
