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
#include <catch/catch.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/*  Overload StringMaker to properly print glm::vec3  */
namespace Catch {
    template<> struct StringMaker<glm::vec3> {
        static std::string convert(glm::vec3 const& value) {
            return glm::to_string(value);
        }
    };
    template<> struct StringMaker<glm::vec4> {
        static std::string convert(glm::vec4 const& value) {
            return glm::to_string(value);
        }
    };
}
