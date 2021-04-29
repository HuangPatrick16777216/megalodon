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
#include <string>
#include "bitboard.hpp"
#include "consts.hpp"
#include "options.hpp"
#include "eval.hpp"
#include "search.hpp"

using Bitboard::popcnt;
using Bitboard::bit;

namespace Eval {
    UCH CENTER_DIST_MAP[64];
    const UCH FILE_DIST_MAP[8] = {3, 2, 1, 0, 0, 1, 2, 3};
    const U64 SURROUNDINGS[64] = {    // Bitboard of surrounding squares up to 2 sq away.
        460551ULL, 986895ULL, 2039583ULL, 4079166ULL,
        8158332ULL, 16316664ULL, 15790320ULL, 14737632ULL,
        117901063ULL, 252645135ULL, 522133279ULL, 1044266558ULL,
        2088533116ULL, 4177066232ULL, 4042322160ULL, 3772834016ULL,
        30182672135ULL, 64677154575ULL, 133666119455ULL, 267332238910ULL,
        534664477820ULL, 1069328955640ULL, 1034834473200ULL, 965845508320ULL,
        7726764066560ULL, 16557351571200ULL, 34218526580480ULL, 68437053160960ULL,
        136874106321920ULL, 273748212643840ULL, 264917625139200ULL, 247256450129920ULL,
        1978051601039360ULL, 4238682002227200ULL, 8759942804602880ULL, 17519885609205760ULL,
        35039771218411520ULL, 70079542436823040ULL, 67818912035635200ULL, 63297651233259520ULL,
        506381209866076160ULL, 1085102592570163200ULL, 2242545357978337280ULL, 4485090715956674560ULL,
        8970181431913349120ULL, 17940362863826698240ULL, 17361641481122611200ULL, 16204198715714437120ULL,
        506381209748635648ULL, 1085102592318504960ULL, 2242545357458243584ULL, 4485090714916487168ULL,
        8970181429832974336ULL, 17940362859665948672ULL, 17361641477096079360ULL, 16204198711956340736ULL,
        506381179683864576ULL, 1085102527893995520ULL, 2242545224314257408ULL, 4485090448628514816ULL,
        8970180897257029632ULL, 17940361794514059264ULL, 17361640446303928320ULL, 16204197749883666432ULL
    };
    void init() {
        for (UCH i = 0; i < 64; i++) CENTER_DIST_MAP[i] = center_dist(i);
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
    float middle_game(const float& pawn_struct, const float& p_attacks, const float& knight,
            const float& rook, const float& queen, const float& king, const float& space) {
        return (
            pawn_struct *  0.9F +
            p_attacks   *  0.9F +
            knight      *  1.F +
            rook        *  1.F +
            queen       *  1.F +
            king        *  1.F +
            space       *  1.F
        );
    }
    float end_game(const float& pawn_struct, const float& p_attacks, const float& knight,
            const float& rook, const float& queen, const float& king, const float& space) {
        return (
            pawn_struct *  1.2F +
            p_attacks   *  0.8F +
            knight      *  0.7F +
            rook        *  1.1F +
            queen       *  0.8F +
            king        * -1.3F +
            space       *  0.0F        // Space encourages pawns in the center, which discourages promotion.
        );
    }

    char center_dist(const char& i) {
        const char x = i&7, y = i>>3;
        const char xdist = (x<=3) ? 3-x : x-4;
        const char ydist = (y<=3) ? 3-y : y-4;
        return xdist + ydist;
    }

    float space(const U64& wp, const U64& bp) {
        float sp = 0;
        for (char x = 2; x < 6; x++) {
            for (char y = 1; y < 5; y++) if (bit(wp, (y<<3)+x)) sp += y-1;
            for (char y = 3; y < 7; y++) if (bit(bp, (y<<3)+x)) sp -= 6-y;
        }
        return sp / 4.0F;
    }
    float pawn_structure(const U64& wp, const U64& bp) {
        // Values represent white - black
        char stacked = 0;
        char islands = 0;
        char passed = 0;
        // Generate file values
        char w_adv[8], b_adv[8];     // Position of most advanced pawn, side dependent (-1 if no pawn)
        char w_back[8], b_back[8];   // Position of least advanced pawn, side dependent (-1 if no pawn)
        U64 w_files[8], b_files[8];  // Bitboards of pawns on each file
        for (UCH i = 0; i < 8; i++) {
            w_files[i] = wp & Bitboard::FILES[i];
            b_files[i] = bp & Bitboard::FILES[i];
            const U64 w = w_files[i] >> i;
            if (w == 0) {
                w_adv[i] = -1;
                w_back[i] = -1;
            } else {
                bool found = false;
                for (char j = 7; j > -1; j--) {
                    if ((w & (1ULL<<(j*8))) != 0) {
                        if (!found) w_adv[i] = j;
                        w_back[i] = j;
                        found = true;
                    }
                }
            }
            const U64 b = b_files[i] >> i;
            if (b == 0) {
                b_adv[i] = -1;
                b_back[i] = -1;
            } else {
                bool found = false;
                for (char j = 7; j > -1; j--) {
                    if ((b & (1ULL<<(j*8))) != 0) {
                        if (!found) b_adv[i] = j;
                        b_back[i] = j;
                        found = true;
                    }
                }
            }
        }
        // Stacked and islands
        bool w_on = false;
        bool b_on = false;
        for (UCH i = 0; i < 8; i++) {
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
        for (UCH i = 0; i < 8; i++) {
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
    float pawn_attacks(const Position& pos) {
        const U64 w_attacks = Bitboard::attacked(pos.wp, 0, 0, 0, 0, 0, 0, true);
        const U64 b_attacks = Bitboard::attacked(pos.bp, 0, 0, 0, 0, 0, 0, false);
        const U64 white = Bitboard::get_white(pos) ^ pos.wp;
        const U64 black = Bitboard::get_black(pos) ^ pos.bp;
        const char w_cnt = popcnt(w_attacks & black);
        const char b_cnt = popcnt(b_attacks & white);
        return 0.25 * (w_cnt-b_cnt);
    }
    float knights(const U64& wn, const U64& bn) {
        float wdist = 0, bdist = 0;
        const char wcnt = popcnt(wn), bcnt = popcnt(bn);
        const bool wp_in_cent = true;//((INNER_CENTER|OUTER_CENTER) & wp) != 0;
        const bool bp_in_cent = true;//((INNER_CENTER|OUTER_CENTER) & bp) != 0;
        for (UCH i = 0; i < 64; i++) {
            if (wp_in_cent && bit(wn, i)) {
                wdist += 6 - CENTER_DIST_MAP[i];
            } else if (bp_in_cent && bit(bn, i)) {
                bdist += 6 - CENTER_DIST_MAP[i];
            }
        }
        if (wcnt > 0) wdist /= wcnt;
        if (bcnt > 0) bdist /= bcnt;
        return wdist - bdist;
    }
    float rooks(const U64& wr, const U64& br, const U64& wp, const U64& bp) {
        float score = 0;
        for (UCH i = 0; i < 64; i++) {
            const UCH x = i&7;
            const U64 w = wp & Bitboard::FILES[x];
            const U64 b = bp & Bitboard::FILES[x];
            const bool open = (w == 0) && (b == 0);
            const bool semi_open = !((w != 0) && (b != 0));
            const char dist = 3 - FILE_DIST_MAP[x];
            if (bit(wr, i)) {
                if (open) score += 0.4F;
                else if (semi_open) score += 0.15F;
                score += (float)(dist) / 20;
            } else if (bit(br, i)) {
                if (open) score -= 0.4F;
                else if (semi_open) score -= 0.15F;
                score -= (float)(dist) / 20;
            }
        }
        return score;
    }
    float queens(const Position& pos) {
        const UCH wq = Bitboard::first_bit(pos.wq).loc, bq = Bitboard::first_bit(pos.bq).loc;
        const U64 white = Bitboard::get_white(pos), black = Bitboard::get_black(pos);
        float score = 0;
        score += (float)(6-CENTER_DIST_MAP[wq]) / 15;
        score -= (float)(6-CENTER_DIST_MAP[bq]) / 15;
        score += (float)(popcnt(SURROUNDINGS[wq]&white)) / 35;
        score -= (float)(popcnt(SURROUNDINGS[bq]&black)) / 35;
        return score;
    }
    float kings(const U64& wk, const U64& bk) {
        const Location w = Bitboard::first_bit(wk);
        const Location b = Bitboard::first_bit(bk);
        const char wdist = CENTER_DIST_MAP[w.loc];
        const char bdist = CENTER_DIST_MAP[b.loc];
        return wdist - bdist;
    }

    float eval(const Options& options, const Position& pos, const vector<Move>& moves, const int& depth, const U64& o_attacks,
            const bool print) {
        if (moves.empty()) {
            bool checked = false;
            if      ( pos.turn && ((o_attacks & pos.wk) != 0)) checked = true;
            else if (!pos.turn && ((o_attacks & pos.bk) != 0)) checked = true;
            if (checked) {
                // Increment value by depth to encourage sooner mate.
                // The larger depth is, the closer it is to the leaf nodes.
                if (pos.turn) return Search::MIN + depth;  // Mate by black
                else return Search::MAX - depth;           // Mate by white
            }
            return 0;
        }
        if (pos.draw50 >= 100) return 0;
        const float mat         =                          material(pos)                           / 1.F;
        const float sp          = options.EvalSpace      * space(pos.wp, pos.bp)                   / 5.F;
        const float pawn_struct = options.EvalPawnStruct * pawn_structure(pos.wp, pos.bp)          / 5.F;
        const float p_attacks   =                          pawn_attacks(pos)                       / 2.F;
        const float knight      = options.EvalKnights    * knights(pos.wn, pos.bn)                 / 16.F;
        const float rook        = options.EvalRooks      * rooks(pos.wr, pos.br, pos.wp, pos.bp)   / 2.F;
        const float queen       = options.EvalQueens     * queens(pos)                             / 6.F;
        const float king        = options.EvalKings      * kings(pos.wk, pos.bk)                   / 16.F;
        // Endgame and middle game are for weighting categories.
        const float mg = middle_game(pawn_struct, p_attacks, knight, rook, queen, king, sp);
        const float eg = end_game(pawn_struct, p_attacks, knight, rook, queen, king, sp);
        const float p = phase(pos);
        const float imbalance = mg*p + eg*(1-p);
        if (print) {
            cout << "       Material | " << mat           << "\n";
            cout << " Pawn Structure | " << pawn_struct   << "\n";
            cout << "   Pawn Attacks | " << p_attacks     << "\n";
            cout << "        Knights | " << knight        << "\n";
            cout << "          Rooks | " << rook          << "\n";
            cout << "         Queens | " << queen         << "\n";
            cout << "          Kings | " << king          << "\n";
            cout << "          Space | " << sp            << "\n\n";
            cout << "    Middle Game | " << mg            << "\n";
            cout << "       End Game | " << eg            << "\n";
            cout << "          Phase | " << p             << "\n";
            cout << "      Imbalance | " << imbalance     << "\n";
            cout << "          Final | " << mat+imbalance << endl;
        }
        return mat + imbalance;
    }
}
