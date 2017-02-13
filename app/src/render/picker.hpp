#pragma once

#include <set>

#include <QObject>
#include <QImage>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include <boost/bimap.hpp>

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
     *
     *  Returns true if anything changed
     */
    bool endUpdate();

    /*
     *  Checks to see if the pointer is any handle type
     */
    bool isHandle(Graph::ValuePtr ptr);

    /*
     *  Installs this handle into the list of handles, marking that
     *  it has been visited (so it won't be erased)
     */
    typedef std::pair<Graph::CellKey, Graph::ValuePtr> HandleKey;
    bool installHandle(const HandleKey& k);

public slots:
    void onViewChanged(QMatrix4x4 mat, QSize size);

protected:

    /*  Here, we store the set of handles to be drawn & picked */
    std::map<HandleKey, Handle*> handles;
    std::set<HandleKey> visited;
    boost::bimap<QRgb, HandleKey> colors;

    /*  Global transform matrix, used to position centers */
    QMatrix4x4 M;

    /*  Projection-only matrix, used to adjust for aspect ratio  */
    QMatrix4x4 proj;

    /*  Window size  */
    QSize window_size;

    /*  img is where pick data is stored  */
    QImage img;
};

}   // namespace Render
}   // namespace App
