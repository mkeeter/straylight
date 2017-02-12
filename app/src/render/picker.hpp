#pragma once

#include <set>

#include <QObject>
#include <QImage>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include "graph/keys.hpp"
#include "graph/ptr.hpp"

namespace App {
namespace Render {

class Handle;

class Picker : public QObject, public QOpenGLFunctions
{
    Q_OBJECT
public:
    Picker();

    /*
     *  Renders all Handles, using the mouse position for highlights
     */
    void draw(QPoint p);

    /*
     *  Returns a handle at the given point, or nullptr
     */
    Handle* pickAt(QPoint p);

    /*
     *  Mark that no handles have yet been seen
     */
    void beginUpdate();

    /*
     *  Erase all unseen handles
     */
    void endUpdate();

    /*
     *  Checks to see if the pointer is any handle type
     */
    bool isHandle(Graph::ValuePtr ptr);

    /*
     *  Installs this handle into the list of handles, marking that
     *  it has been visited (so it won't be erased)
     */
    typedef std::pair<Graph::CellKey, Graph::ValuePtr> HandleKey;
    void installHandle(const HandleKey& k);

public slots:
    void onViewChanged(QMatrix4x4 mat, QSize size);

protected:

    /*  Here, we store the set of handles to be drawn & picked */
    std::map<HandleKey, Handle*> handles;
    std::set<HandleKey> visited;
    std::map<QRgb, Handle*> colors;

    // Properties used in drawing and rendering
    QMatrix4x4 M;
    QSize window_size;

    //  This is the image where pick data is stored
    QImage img;
};

}   // namespace Render
}   // namespace App
