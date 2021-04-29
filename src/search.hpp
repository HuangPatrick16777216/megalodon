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

struct SearchInfo {
    SearchInfo();
    SearchInfo(const int&, const int&, const float&, const U64&, const int&, const int&, const double&, const vector<Move>&,
        const float&, const float&, const bool&);
    string as_string();
    bool is_mate();
    int depth;
    int seldepth;
    float score;
    U64 nodes;
    int nps;
    int hashfull;
    double time;
    vector<Move> pv;
    float alpha;
    float beta;
    bool full;
};
namespace Search {
    constexpr float MAX = 10000;
    constexpr float MIN = -10000;
    constexpr float MATE_BOUND_MAX = MAX - 100;
    constexpr float MATE_BOUND_MIN = MIN + 100;
    float move_time(const Position&, const float&, const float&);
    SearchInfo search(const Options&, const Position&, const int&, const double&, const bool&,
        bool&);
}
