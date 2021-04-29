//
//  Megalodon
//  UCI chess engine
//  Copyright the Megalodon developers
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "bitboard.hpp"
#include "consts.hpp"
#include "options.hpp"

namespace Eval {
    constexpr int MIDGAME_LIM = 50;
    constexpr int ENDGAME_LIM = 20;
    constexpr U64 INNER_CENTER = 103481868288ULL;
    constexpr U64 OUTER_CENTER = 66125924401152ULL;
    void init();
    char center_dist(const char&);
    float material(const Position&);
    float total_mat(const Position&);
    float non_pawn_mat(const Position&);
    float eval(const Options&, const Position&, const vector<Move>&, const int&, const U64&, const bool=false);
}
