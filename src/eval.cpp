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

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "bitboard.hpp"
#include "options.hpp"
#include "eval.hpp"
#include "search.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

using Bitboard::popcnt;
using Bitboard::bit;


float material(const Position& pos) {
    float value = 0;
    value += popcnt(pos.wp) * 1;
    value += popcnt(pos.wn) * 3;
    value += popcnt(pos.wb) * 3;
    value += popcnt(pos.wr) * 5;
    value += popcnt(pos.wq) * 9;
    value -= popcnt(pos.bp) * 1;
    value -= popcnt(pos.bn) * 3;
    value -= popcnt(pos.bb) * 3;
    value -= popcnt(pos.br) * 5;
    value -= popcnt(pos.bq) * 9;
    return value;
}

float total_mat(const Position& pos) {
    float value = 0;
    value += popcnt(pos.wp) * 1;
    value += popcnt(pos.wn) * 3;
    value += popcnt(pos.wb) * 3;
    value += popcnt(pos.wr) * 5;
    value += popcnt(pos.wq) * 9;
    value += popcnt(pos.bp) * 1;
    value += popcnt(pos.bn) * 3;
    value += popcnt(pos.bb) * 3;
    value += popcnt(pos.br) * 5;
    value += popcnt(pos.bq) * 9;
    return value;
}

float non_pawn_mat(const Position& pos) {
    float value = 0;
    value += popcnt(pos.wn) * 3;
    value += popcnt(pos.wb) * 3;
    value += popcnt(pos.wr) * 5;
    value += popcnt(pos.wq) * 9;
    value += popcnt(pos.bn) * 3;
    value += popcnt(pos.bb) * 3;
    value += popcnt(pos.br) * 5;
    value += popcnt(pos.bq) * 9;
    return value;
}

float phase(const Position& pos) {
    // 1 = full middlegame, 0 = full endgame.
    float npm = non_pawn_mat(pos);
    if (npm >= MIDGAME_LIM) return 1;
    else if (npm <= ENDGAME_LIM) return 0;
    else return (float)(npm-ENDGAME_LIM) / LIM_DIFF;
}

float middle_game(const float& pawn_struct) {
    return (
        pawn_struct * 0.9
    );
}

float end_game(const float& pawn_struct) {
    return (
        pawn_struct * 1.2
    );
}


float pawn_structure(const U64& pawns) {
    // Values represent white_count - black_count
    char passed = 0;
    char backward = 0;
    char islands = 0;
    char stacked = 0;

    // Islands and stacked
    bool white = false;  // Whether the current index is a pawn.
    for (char i = 0; i < 8; i++) {
        const U64& p = Bitboard::FILES[i] & pawns;
        if (p == Bitboard::EMPTY) {
            white = false;
        } else {
            if (!white) islands++;
            white = true;
        }

        const char& cnt = popcnt(p);
        if (cnt > 1) stacked += (cnt-1);
    }

    return (
        -0.3 * islands +
        -0.2 * stacked
    );
}


float eval(const Options& options, const Position& pos, const bool& moves_exist, const int& depth, const U64& o_attacks) {
    if (!moves_exist) {
        bool checked;
        if (pos.turn && ((o_attacks & pos.wk) != 0)) checked = true;
        else if (!pos.turn && ((o_attacks & pos.bk) != 0)) checked = true;
        if (checked) {
            // Increment value by depth to encourage sooner mate.
            if (pos.turn) return MIN+depth;
            else return MAX-depth;
        }
        return 0;
    }
    float score = 0;

    const float mat = material(pos);
    const float pawn_struct = pawn_structure(pos.wp) - pawn_structure(pos.bp);

    // Endgame and middle game are for weighting categories.
    const float mg = middle_game(pawn_struct);
    const float eg = end_game(pawn_struct);
    const float p = phase(pos);
    score = mg*p + eg*(1 - p);

    return mat + score;
}
