#pragma once

#include <set>

#include <QObject>
#include <QImage>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include <boost/bimap.hpp>

#include "graph/types/keys.hpp"

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
    enum DrawMode { DRAW_PICKER, DRAW_NORMAL, DRAW_HOVER, DRAW_DRAG };
    void draw(QPoint p, const Graph::Env& env, DrawMode mode=DRAW_NORMAL);

    /*
     *  Returns a handle at the given point, or nullptr
     */
    Handle* pickAt(QPoint p);

    /*
     *  Installs this handle into the picker for rendering
     */
    void installHandle(const Graph::CellKey& k, Handle* h);

    /*
     *  Erase any handle that's not in hs
     */
    void prune(const std::set<Graph::CellKey>& hs);

    /*
     *  Loads the picker image!
     */
    void setImage(QImage i) { img = i; }

    /*
     *  Returns the picker image width
     *  (used to compensate for high-DPI screens)
     */
    float width() const { return img.width(); }

    /*
     *  Adjusts M and window_size
     */
    void setView(QMatrix4x4 mat, QSize size);

    /*
     *  Looks up a handle by key
     */
    Handle* getHandle(const Graph::CellKey& k) const;

protected:
    /*  Here, we store the set of handles to be drawn & picked */
    std::map<Graph::CellKey, Handle*> handles;

    /*  These are colors assigned to each handle for picking  */
    boost::bimap<QRgb, Handle*> colors;

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
