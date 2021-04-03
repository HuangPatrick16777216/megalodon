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


namespace Eval {
    char CENTER_DIST_MAP[64];

    void init() {
        for (char i = 0; i < 64; i++) CENTER_DIST_MAP[i] = center_dist(i);
    }


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
        const float npm = non_pawn_mat(pos);
        if (npm >= MIDGAME_LIM) return 1;
        else if (npm <= ENDGAME_LIM) return 0;
        else return ((float)(npm-ENDGAME_LIM) / (MIDGAME_LIM-ENDGAME_LIM));
    }

    float middle_game(const float& pawn_struct, const float& knight, const float& king, const float& space) {
        return (
            pawn_struct * 0.9 +
            knight      * 0.2 +
            king        * 0.2 +
            space       * 1
        );
    }

    float end_game(const float& pawn_struct, const float& knight, const float& king, const float& space) {
        return (
            pawn_struct * 1.2 +
            knight      * 0.1 +
            king        * 0.2 +
            space       * 0        // Space encourages pawns in the center, which discourages promotion.
        );
    }


    char center_dist(const char& i) {
        const char x = i&7, y = i>>3;
        char dist = 0;
        if (x <= 3) dist += 3-x;
        else dist += x-4;
        if (y <= 3) dist += 3-y;
        else dist += y-4;
        return dist;
    }

    char manhattan_dist(const char& x1, const char& y1, const char& x2, const char& y2) {
        return abs(x1-x2) + abs(y1-y2);
    }


    float pawn_structure(const U64& wp, const U64& bp) {
        // Values represent white - black
        char stacked = 0;
        char islands = 0;
        char passed = 0;
        char backwards = 0;

        // Generate file values
        char w_adv[8], b_adv[8];     // Position of most advanced pawn, side dependent (-1 if no pawn)
        char w_back[8], b_back[8];   // Position of least advanced pawn, side dependent (-1 if no pawn)
        U64 w_files[8], b_files[8];  // Bitboards of pawns on each file
        for (char i = 0; i < 8; i++) {
            w_files[i] = wp & Bitboard::FILES[i];
            b_files[i] = bp & Bitboard::FILES[i];

            const U64 w = (wp & Bitboard::FILES[i]) >> i;
            bool found = false;
            for (char j = 7; j > -1; j--) {
                if (bit(w, j<<3)) {
                    if (!found) w_adv[i] = j;
                    w_back[i] = j;
                    found = true;
                }
            }
            if (!found) {
                w_adv[i] = -1;
                w_back[i] = -1;
            }

            const U64 b = (bp & Bitboard::FILES[i]) >> i;
            found = false;
            for (char j = 7; j > -1; j--) {
                if (bit(b, j<<3)) {
                    if (!found) b_adv[i] = j;
                    b_back[i] = j;
                    found = true;
                }
            }
            if (!found) {
                b_adv[i] = -1;
                b_back[i] = -1;
            }
        }

        // Stacked and islands
        bool w_on = false;
        bool b_on = false;
        for (char i = 0; i < 8; i++) {
            if (w_files[i] == 0) w_on = false;
            else {
                if (!w_on) islands++;
                w_on = true;
            }
            if (b_files[i] == 0) b_on = false;
            else {
                if (!b_on) islands--;
                b_on = true;
            }

            const char wcnt = popcnt(w_files[i]);
            const char bcnt = popcnt(b_files[i]);
            if (wcnt >= 2) stacked += (wcnt-1);
            if (bcnt >= 2) stacked -= (bcnt-1);
        }

        // Passed
        for (char i = 0; i < 8; i++) {
            if (w_files[i] != 0) {
                const bool blocked_right = (i>=1) && (b_back[i-1]!=-1) && (w_adv[i]>=b_back[i-1]);
                const bool blocked_left = (i<=6) && (b_back[i+1]!=-1) && (w_adv[i]>=b_back[i+1]);
                if (!blocked_right && !blocked_left) passed++;
            }
            if (b_files[i] != 0) {
                const bool blocked_right = (i>=1) && (w_back[i-1]!=-1) && (b_adv[i]<=w_back[i-1]);
                const bool blocked_left = (i<=6) && (w_back[i+1]!=-1) && (b_adv[i]<=w_back[i+1]);
                if (!blocked_right && !blocked_left) passed--;
            }
        }

        return (
            -0.3 * islands +
            -0.2 * stacked +
            0.6 * passed
        );
    }

    float space(const U64& wp, const U64& bp) {
        float space = 0;

        for (char x = 2; x < 6; x++) {
            for (char y = 1; y < 5; y++) {  // White
                if (bit(wp, (y<<3)+x)) space += y-1;
            }
            for (char y = 3; y < 7; y++) {  // Black
                if (bit(bp, (y<<3)+x)) space -= 6-y;
            }
        }

        return space / 6;
    }

    float knights(const U64& wn, const U64& bn, const U64& wp, const U64& bp) {
        float wdist = 0, bdist = 0;
        char wcnt = 0, bcnt = 0;
        const bool wp_in_cent = ((INNER_CENTER|OUTER_CENTER) & wp) != 0;
        const bool bp_in_cent = ((INNER_CENTER|OUTER_CENTER) & bp) != 0;

        for (char i = 0; i < 64; i++) {
            if (wp_in_cent && bit(wn, i)) {
                wcnt++;
                wdist += 6 - CENTER_DIST_MAP[i];
            } else if (bp_in_cent && bit(bn, i)) {
                bcnt++;
                bdist += 6 - CENTER_DIST_MAP[i];
            }
        }

        if (wcnt > 0) wdist /= wcnt;
        if (bcnt > 0) bdist /= bcnt;
        return wdist - bdist;
    }

    float kings_mg(const U64& wk, const U64& bk, const U64& wpieces, const U64& bpieces, const U64& wp, const U64& bp) {
        // Distance to center
        const Location w = Bitboard::first_bit(wk);
        const Location b = Bitboard::first_bit(bk);
        const char wdist = CENTER_DIST_MAP[w.loc];
        const char bdist = CENTER_DIST_MAP[b.loc];

        // Pawn shield
        float shield = 0;
        for (char x = w.x-1; x <= w.x+1; x++) {
            if (w.y <= 6) {
                if (bit(wp, w.loc+8)) shield += 1.2;
                if (w.y <= 5) {
                    if (bit(wp, w.loc+16)) shield += 0.6;
                }
            }
        }
        for (char x = b.x-1; x <= b.x+1; x++) {
            if (b.y >= 1) {
                if (bit(bp, b.loc-8)) shield -= 1.2;
                if (b.y >= 2) {
                    if (bit(bp, b.loc-16)) shield -= 0.6;
                }
            }
        }

        // Piece attacking and defending
        float attack = 0;
        //float defend = 0;
        const char wcnt = popcnt(wpieces), bcnt = popcnt(bpieces);
        for (char x = 0; x < 8; x++) {
            for (char y = 0; y < 8; y++) {
                const char loc = (y<<3) + x;
                const char wdist = manhattan_dist(x, y, w.x, w.y);
                const char bdist = manhattan_dist(x, y, b.x, b.y);
                if (bit(wpieces, loc)) {
                    //defend += 16 - wdist;
                    attack += 16 - bdist;
                } else if (bit(bpieces, loc)) {
                    //defend -= 16 - bdist;
                    attack -= 16 - wdist;
                }
            }
        }
        if (wcnt+bcnt > 0) {
            attack /= (wcnt+bcnt);
            //defend /= (wcnt+bcnt);
        }

        return (
            wdist-bdist +
            shield * 1.2 +
            attack / 1.7
        );
    }

    float kings_eg(const U64& wk, const U64& bk) {
        // Distance to center
        const Location w = Bitboard::first_bit(wk);
        const Location b = Bitboard::first_bit(bk);
        const char wdist = 8 - CENTER_DIST_MAP[w.loc];
        const char bdist = 8 - CENTER_DIST_MAP[b.loc];

        return wdist - bdist;
    }


    float eval(const Options& options, const Position& pos, const vector<Move>& moves, const int& depth, const U64& o_attacks) {
        if (moves.empty()) {
            bool checked;
            if (pos.turn && ((o_attacks & pos.wk) != 0)) checked = true;
            else if (!pos.turn && ((o_attacks & pos.bk) != 0)) checked = true;
            if (checked) {
                // Increment value by depth to encourage sooner mate.
                // The larger depth is, the closer it is to the leaf nodes.
                if (pos.turn) return Search::MIN + depth;  // Mate by black
                else return Search::MAX - depth;           // Mate by white
            }
            return 0;
        }

        const U64 wpieces = pos.wn | pos.wb | pos.wr | pos.wq;
        const U64 bpieces = pos.bn | pos.bb | pos.br | pos.bq;

        const float mat = material(pos);
        const float pawn_struct = (float)options.EvalPawnStruct/100.F * pawn_structure(pos.wp, pos.bp);
        const float knight      = (float)options.EvalKnights/100.F    * knights(pos.wn, pos.bn, pos.wp, pos.bp);
        const float king_mg     = (float)options.EvalKings/100.F      * kings_mg(pos.wk, pos.bk, wpieces, bpieces, pos.wp, pos.bp);
        const float king_eg     = (float)options.EvalKings/100.F      * kings_eg(pos.wk, pos.bk);
        const float sp          = (float)options.EvalSpace/100.F      * space(pos.wp, pos.bp);

        // Endgame and middle game are for weighting categories.
        const float mg = middle_game(pawn_struct, knight, king_mg, sp);
        const float eg = end_game(pawn_struct, knight, king_eg, sp);
        const float p = phase(pos);
        const float imbalance = mg*p + eg*(1-p);

        return mat + 0.5*imbalance;
    }
}
