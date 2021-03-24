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

float non_pawn_mat(const Position& pos, const bool& mg) {
    float value = 0;
    if (mg) {
        value += popcnt(pos.wn) * MG_KNIGHT;
        value += popcnt(pos.wb) * MG_BISHOP;
        value += popcnt(pos.wr) * MG_ROOK;
        value += popcnt(pos.wq) * MG_QUEEN;
        value += popcnt(pos.bn) * MG_KNIGHT;
        value += popcnt(pos.bb) * MG_BISHOP;
        value += popcnt(pos.br) * MG_ROOK;
        value += popcnt(pos.bq) * MG_QUEEN;
    } else {
        value += popcnt(pos.wn) * EG_KNIGHT;
        value += popcnt(pos.wb) * EG_BISHOP;
        value += popcnt(pos.wr) * EG_ROOK;
        value += popcnt(pos.wq) * EG_QUEEN;
        value += popcnt(pos.bn) * EG_KNIGHT;
        value += popcnt(pos.bb) * EG_BISHOP;
        value += popcnt(pos.br) * EG_ROOK;
        value += popcnt(pos.bq) * EG_QUEEN;
    }
    return value;
}


float phase(const Position& pos) {
    // 1 = full middlegame, 0 = full endgame.
    float npm = non_pawn_mat(pos, true);
    if (npm >= MIDGAME_LIM) return 1;
    else if (npm <= ENDGAME_LIM) return 0;
    else return (npm-ENDGAME_LIM / (MIDGAME_LIM-ENDGAME_LIM));
}

float middle_game(const Position& pos) {
    float score = 0;

    return score;
}

float end_game(const Position& pos) {
    float score = 0;

    return score;
}


float pawn_structure(const U64& wp, const U64& bp) {
    // Values represent white_count - black_count
    char passed = 0;
    char backward = 0;
    char islands = 0;
    char doubled = 0;

    // Islands and doubled/tripled
    bool white = false, black = false;  // Whether the current index is a pawn.
    for (char i = 0; i < 8; i++) {
        U64 w = Bitboard::FILES[i] & wp;
        U64 b = Bitboard::FILES[i] & bp;
        if (w == 0) {
            white = false;
        } else {
            if (!white) islands++;
            white = true;
        }
        if (b == 0) {
            black = false;
        } else {
            if (!black) islands--;
            black = true;
        }

        char wcnt = popcnt(w);
        char bcnt = popcnt(b);
        if (wcnt >= 2) doubled += (wcnt-1);
        if (bcnt >= 2) doubled -= (bcnt-1);
    }

    return (
        -0.3 * islands +
        -0.2 * doubled
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

    const float mg = middle_game(pos), eg = end_game(pos);
    const float p = phase(pos);
    const float score = mg*p + eg*(1-p);

    return score;
}
