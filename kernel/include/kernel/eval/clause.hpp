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
#pragma once

#include "kernel/tree/opcode.hpp"

/*
 *  A clause is used in an Evaluator to evaluate a tree
 */
struct Clause
{
    typedef uint32_t Id;

    /*
     *  Clause constructor
     */
    Clause(Opcode::Opcode op, uint32_t id, uint32_t a, uint32_t b)
        : op(op), id(id), a(a), b(b) {}

    /*  Opcode for this clause  */
    const Opcode::Opcode op;

    /*  Populated for operators with arguments */
    Id const id;
    Id const a;
    Id const b;
};
