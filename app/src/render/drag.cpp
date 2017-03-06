#include <QVector2D>

#include "app/render/drag.hpp"

namespace App {
namespace Render {

QPair<QVector3D, QVector3D> Drag::toWorld(const QMatrix4x4& M,
                                          const QVector2D& cursor,
                                          const QVector3D start)
{
    const QVector3D offset = M * QVector3D(0, 0, 0);
    const QVector3D ray = (M * QVector3D(0, 0, -1) - offset).normalized();

    // Position pos to minimize travel along the ray from start point
    // TODO: clean up this vector math
    const QVector3D _pos = M * QVector3D(cursor.x(), cursor.y(), 0);
    const QVector3D pos = _pos + ray * QVector3D::dotProduct(start - _pos, ray);

    return {pos, ray};
}

}   // namespace Render
}   // namespace App
