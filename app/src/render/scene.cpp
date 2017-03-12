#include "app/render/scene.hpp"
#include "app/render/canvas.hpp"
#include "app/render/drag.hpp"
#include "app/render/point_handle.hpp"
#include "app/render/shape_handle.hpp"

#include "app/bridge/escaped.hpp"
#include "app/bridge/bridge.hpp"
#include "app/bridge/graph.hpp"

#include "app/bind/bind_s7.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace App {
namespace Render {

Scene::Scene()
    : QQuickFramebufferObject()
{
    App::Bridge::GraphModel::instance()->installScene(this);
}

QQuickFramebufferObject::Renderer* Scene::createRenderer() const
{
    auto c = new Canvas();
    return c;
}

void Scene::rotateIncremental(float dx, float dy)
{
    pitch += dy;
    yaw += dx;

    pitch = fmax(fmin(pitch, 180), 0);
    yaw = fmod(yaw, 360);
    updateRenderer();
    update();
}

void Scene::panIncremental(float dx, float dy)
{
    // Find the starting position in world coordinates
    auto inv = M().inverted();
    auto diff = inv.map({float(dx/width()), float(dy/height()), 0}) -
                inv.map({0, 0, 0});

    center += diff*2;
    updateRenderer();
    update();
}

void Scene::zoomIncremental(float ds, float x, float y)
{
    QVector3D pt(x / width() - 0.5, y / height() - 0.5, 0);
    auto a = M().inverted().map(pt);

    scale *= pow(1.1, ds / 120.);
    center += 2 * (M().inverted().map(pt) - a);
    updateRenderer();
    update();
}

void Scene::updateRenderer()
{
    for (auto& s : shapes)
    {
        s.second->enqueue(M(), QSize(width(), height()));
    }
}

void Scene::mouseMove(float x, float y)
{
    if (mouse_state == DRAG_ROT)
    {
        rotateIncremental(x - mouse_pos.x(), y - mouse_pos.y());
    }
    else if (mouse_state == DRAG_PAN)
    {
        panIncremental(x - mouse_pos.x(), y - mouse_pos.y());
    }
    else if (mouse_state == DRAG_HANDLE)
    {
        QVector2D mouse_gl(
            2 * mouse_pos.x() / width() - 1,
            2 * mouse_pos.y() / height() - 1);
        assert(mouse_drag != nullptr);
        auto sol = mouse_drag->dragTo(M().inverted(), mouse_gl);
        App::Bridge::GraphModel::instance()->setVariables(sol);
    }
    else
    {
        update();
    }
    mouse_pos = {x, y};
}

void Scene::mouseClick(int button)
{
    if (mouse_state == RELEASED)
    {
        if (button == Qt::LeftButton)
        {
            mouse_state = CLICK_LEFT;
            update();
        }
        else if (button == Qt::RightButton)
        {
            mouse_state = CLICK_RIGHT;
            update();
        }
    }
}

void Scene::mouseRelease()
{
    if (mouse_state == DRAG_HANDLE)
    {
        mouse_drag = nullptr;
    }
    mouse_state = RELEASED;
    picker_changed = true;
    update();
}

void Scene::updatePicker()
{
    picker_changed = true;
    update();
}

QMatrix4x4 Scene::proj() const
{
    QMatrix4x4 m;

    //  Compress the Z axis to avoid clipping
    //  The value 4 isn't anything magical here, just seems to work well
    const float Z_COMPRESS = 4;
    const float frac = width() / float(height());
    if (frac > 1)
    {
        m.scale(1/frac, 1, 1/Z_COMPRESS);
    }
    else
    {
        m.scale(1, frac, 1/Z_COMPRESS);
    }
    return m;
}

QMatrix4x4 Scene::view() const
{
    QMatrix4x4 m;
    m.scale(scale, scale, scale);
    m.rotate(pitch, {1, 0, 0});
    m.rotate(yaw,   {0, 0, 1});
    m.translate(center);

    return m;
}

////////////////////////////////////////////////////////////////////////////////

void Scene::updateFrom(const Graph::Response& r)
{
    bool changed = false;

    switch (r.op)
    {
        case Graph::Response::IO_VALUE_CHANGED:
        {
            Graph::CellKey key(r.env, Graph::CellIndex(r.other));

            if (handles.count(key))
            {
                changed |= handles.at(key)->setIO(true);
            }
            break;
        }
        case Graph::Response::IO_INPUT_CREATED:
        case Graph::Response::IO_OUTPUT_CREATED:
        {
            const Graph::CellIndex cell(r.other);
            if (cells.count(cell))
            {
                for (const auto& key : cells.at(cell))
                {
                    if (handles.count(key))
                    {
                        changed |= handles.at(key)->setIO(true);
                    }
                }
            }
            break;
        }
        case Graph::Response::IO_DELETED:
        {
            const Graph::CellIndex cell(r.other);
            if (cells.count(cell))
            {
                for (const auto& key : cells.at(cell))
                {
                    if (handles.count(key))
                    {
                        changed |= handles.at(key)->setIO(false);
                    }
                }
            }
            break;
        }
        case Graph::Response::CELL_ERASED:
        {
            const Graph::CellIndex cell(r.target);
            if (cells.count(cell))
            {
                for (const auto& k : cells.at(cell))
                {
                    if (shapes.count(k))
                    {
                        shapes.at(k)->deleteWhenNotRunning();
                        shapes.erase(k);
                    }
                    if (handles.count(k))
                    {
                        handles.erase(k);
                    }
                }
                cells.erase(cell);
                changed = true;
            }
            break;
        }
        case Graph::Response::VALUE_CHANGED:
        {
            const Graph::CellKey k { r.env, Graph::CellIndex(r.target) };
            cells[k.second].insert(k);

            // We replace the renderer without fail, so always delete a
            // pre-existing renderer if there's one in place for this key.
            if (shapes.count(k))
            {
                shapes.at(k)->deleteWhenNotRunning();
                shapes.erase(k);
                if (handles.count(k))
                {
                    assert(dynamic_cast<ShapeHandle*>(handles.at(k)));
                    handles.erase(k);
                }
                changed = true;
            }

            // If the value is a shape, then convert it into a renderer and
            // wire it into the Scene, kicking off an initial render.
            if (auto n = dynamic_cast<App::Bridge::EscapedShape*>(r.value))
            {
                auto ren = new App::Render::Renderer(n->eval);
                shapes[k] = ren;
                connect(ren, &App::Render::Renderer::done,
                        this, &Scene::update);
                ren->enqueue(M(), QSize(width(), height()));

                changed = true;
            }

            // If the value is an EscapedHandle, then turn it into a real
            // Handle and update its values, etc.
            if (auto p = dynamic_cast<App::Bridge::EscapedHandle*>(r.value))
            {
                // If handle type mismatch, delete
                if (handles.count(k) && handles.at(k)->tag() != p->tag())
                {
                    handles.erase(k);
                    changed = true;
                }
                // If not present, create a new handle here
                if (!handles.count(k))
                {
                    if (dynamic_cast<Bridge::EscapedPointHandle*>(p))
                    {
                        handles.insert({k, new PointHandle()});
                    }
                    else if (dynamic_cast<Bridge::EscapedShape*>(p))
                    {
                        handles.insert({k, new ShapeHandle()});
                    }
                    else
                    {
                        assert(false);  // Unknown handle type!
                    }
                    picker_changed = true;
                    changed = true;
                }
                //  Update the handle from the EscapedHandle
                changed |= handles.at(k)->updateFrom(p);
            }
            break;
        }

        default:    assert(false);
    }

    if (changed)
    {
        update();
    }
}


void Scene::setEnv(Graph::Env env_)
{
    if (env_ != env)
    {
        env = env_;
        update();
    }
}

}   // namespace Render
}   // namespace App
