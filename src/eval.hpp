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
#include "options.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


constexpr int MIDGAME_LIM = 15258;
constexpr int ENDGAME_LIM = 3915;

constexpr int MG_PAWN = 124;
constexpr int MG_KNIGHT = 781;
constexpr int MG_BISHOP = 825;
constexpr int MG_ROOK = 1276;
constexpr int MG_QUEEN = 2538;

constexpr int EG_PAWN = 206;
constexpr int EG_KNIGHT = 854;
constexpr int EG_BISHOP = 915;
constexpr int EG_ROOK = 1380;
constexpr int EG_QUEEN = 2682;


float total_mat(const Position&);
float non_pawn_mat(const Position&, const bool&);

float phase(const Position&);

float middle_game(const Position&);
float end_game(const Position&);

float eval(const Options&, const Position&, const bool&, const int&, const U64&);
