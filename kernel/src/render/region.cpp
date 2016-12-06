/*
 *  Copyright (C) 2016 Matthew Keeter  <matt.j.keeter@gmail.com>
 *
 *  This file is part of the Ao library.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  Ao is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Ao.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>

#include "kernel/render/region.hpp"

Region::Region(Interval x, Interval y, Interval z, float res)
    : Region(x, y, z, res, res, res)
{
    // Nothing to do here
}

Region::Region(Interval x, Interval y, Interval z,
               float rx, float ry, float rz)
    : X(x, rx), Y(y, ry), Z(z, rz)
{
    // Nothing to do here
}

Region::Region(Axis x, Axis y, Axis z)
    : X(x), Y(y), Z(z)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

Subregion Region::view() const
{
    return Subregion(*this);
}

Region Region::powerOfTwo(int dims) const
{
    assert(dims == 2 || dims == 3);
    if (dims == 2)
    {
        assert(Z.values.size() == 1);
    }

    size_t vox = std::max({X.values.size(), Y.values.size(), Z.values.size()});
    size_t n = 1 << (size_t)ceil(log(vox) / log(2));

    // Calculate how much extra needs to be added to an axis
    auto d = [=](const Axis& a){
        return (a.bounds.upper() - a.bounds.lower()) *
               (float(n) / a.values.size() - 1);
    };

    // Return the expanded axis bounds
    auto a = [=](const Axis& a){
        auto da = d(a);
        return Axis({a.bounds.lower() - da/2,
                     a.bounds.upper() + da/2}, n);
    };

    auto r = Region(a(X), a(Y), dims == 3 ? a(Z) : Z);

    assert(r.X.values.size() == n);
    assert(r.Y.values.size() == n);
    assert(r.Z.values.size() == (dims == 3) ? n : 1);

    return r;
}

////////////////////////////////////////////////////////////////////////////////

Region::Axis::Axis(Interval i, float res)
    : bounds(expand(i, res))
{
    // bounds should be an exact multiple of 1/res, but round to
    // compensate for floating-point inaccuracies
    size_t size = std::max((size_t)1,
            (size_t)std::round(res * (bounds.upper() - bounds.lower())));
    for (unsigned index=0; index < size; ++index)
    {
        const float frac = (index + 0.5) / size;
        values.push_back(bounds.lower() * (1 - frac) + bounds.upper() * frac);
    }
}

Region::Axis::Axis(Interval i, size_t size)
    : bounds(i)
{
    for (unsigned index=0; index < size; ++index)
    {
        const float frac = (index + 0.5) / size;
        values.push_back(i.lower() * (1 - frac) + i.upper() * frac);
    }
}

Interval Region::Axis::expand(Interval i, float res)
{
    if (res == 0)
    {
        return i;
    }
    else
    {
        size_t size = std::ceil(res * (i.upper() - i.lower()));
        float extra = size / res - (i.upper() - i.lower());
        return {i.lower() - extra/2, i.upper() + extra/2};
    }
}
