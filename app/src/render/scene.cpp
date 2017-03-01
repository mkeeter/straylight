#include "app/render/scene.hpp"
#include "app/render/canvas.hpp"
#include "app/render/drag.hpp"

namespace App {
namespace Render {

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
    update();
}

void Scene::panIncremental(float dx, float dy)
{
    // Find the starting position in world coordinates
    auto inv = M().inverted();
    auto diff = inv.map({float(dx/width()), float(dy/height()), 0}) -
                inv.map({0, 0, 0});

    center += diff*2;
    update();
}

void Scene::zoomIncremental(float ds, float x, float y)
{
    QVector3D pt(x / width() - 0.5, y / height() - 0.5, 0);
    auto a = M().inverted().map(pt);

    scale *= pow(1.1, ds / 120.);
    center += 2 * (M().inverted().map(pt) - a);
    update();
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
#if 0   // TODO
        App::Core::Bridge::instance()->setVariables(sol);
#endif
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

}   // namespace Render
}   // namespace App
