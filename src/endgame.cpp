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
#include "endgame.hpp"
#include "eval.hpp"


namespace Endgame {
    bool cnt_match(const vector<char> c1, const vector<char> c2) {
        for (UCH i = 0; i < c1.size(); i++) {
            if (c1[i] != c2[i]) return false;
        }
        return true;
    }
    vector<char> get_cnts(const Position& pos) {
        const char wpc = Bitboard::popcnt(pos.wp);
        const char wnc = Bitboard::popcnt(pos.wn);
        const char wbc = Bitboard::popcnt(pos.wb);
        const char wrc = Bitboard::popcnt(pos.wr);
        const char wqc = Bitboard::popcnt(pos.wq);
        const char bpc = Bitboard::popcnt(pos.bp);
        const char bnc = Bitboard::popcnt(pos.bn);
        const char bbc = Bitboard::popcnt(pos.bb);
        const char brc = Bitboard::popcnt(pos.br);
        const char bqc = Bitboard::popcnt(pos.bq);
        return {wpc, wnc, wbc, wrc, wqc, bpc, bnc, bbc, brc, bqc};
    }
    int eg_type(const Position& pos) {
        const vector<char> counts = get_cnts(pos);
        if (pos.turn && cnt_match(W_KQvK, counts)) {
            return 1;
        } else if (!pos.turn && cnt_match(B_KQvK, counts)) {
            return 1;
        }
        return 0;
    }

    Move bestmove(const Position& pos, const vector<Move>& moves, const int& eg) {
        const vector<char> counts = get_cnts(pos);
        if (pos.turn) {
            switch (eg) {
                case 1: return kqvk(moves, pos, pos.wk, pos.wq, pos.bk);
            }
        } else {
            switch (eg) {
                case 1: return kqvk(moves, pos, pos.bk, pos.bq, pos.wk);
            }
        }
        return Move();
    }

    Move kqvk(const vector<Move>& moves, const Position& pos, const U64& _ck, const U64& _cq, const U64& _ok) {
        const Location ck = Bitboard::first_bit(_ck);
        const Location cq = Bitboard::first_bit(_cq);
        const Location ok = Bitboard::first_bit(_ok);
        const char ckp = ck.loc;
        const char cqp = cq.loc;
        const char okp = ok.loc;
        char corner_dist = 16;
        Location closest_corner = CORNERS[0];
        for (const auto& corner: CORNERS) {
            const Location cor(corner);
            const char dist = abs(ok.x-cor.x) + abs(ok.y-cor.y);
            if (dist < corner_dist) {
                corner_dist = dist;
                closest_corner = cor;
            }
        }
        if (corner_dist <= 1 && ((abs(cq.x-ok.x)==1 && abs(cq.y-closest_corner.y) == 3) || (abs(cq.y-ok.y)==1 && abs(cq.x-closest_corner.x) == 3))) {
            const char dx = abs(cq.x-ok.x);
            const char dy = abs(cq.y-ok.y);
            if ((dx==1 && (dy==2||dy==3)) || (dy==1 && (dx==2||dx==3))) {  // King is cornered by queen.
                Location k_target;
                if (closest_corner.x > 0) k_target.x = 5;
                else k_target.x = 2;
                if (closest_corner.y > 0) k_target.y = 5;
                else k_target.y = 2;
                const char k_target_pos = (k_target.y<<3) + k_target.x;
                if (k_target_pos == ckp) {  // Do checkmating move
                    for (const auto& move: moves) {
                        const char ok_diff = abs(okp-move.to);
                        const char ck_diff = abs(ckp-move.to);
                        if ((ok_diff==9 || ok_diff==7) && (ck_diff==9 || ck_diff==7)) return move;
                    }
                } else {  // Move king closer
                    char closest = 16;
                    Move best_move = moves[0];
                    for (const auto& move: moves) {
                        if (move.from == ckp) {
                            const Location to(move.to);
                            const char delta = abs(k_target.x-to.x) + abs(k_target.y-to.y);
                            if (delta < closest) {
                                closest = delta;
                                best_move = move;
                            }
                        }
                    }
                    return best_move;
                }
            }
        }
        char best_dist = 16;
        char least_move_dist = 16;
        Move best_move = moves[0];
        for (const auto& move: moves) {
            if (move.from == cqp) {
                const Position new_pos = Bitboard::push(pos, move);
                const vector<Move> new_moves = Bitboard::legal_moves(new_pos, Bitboard::attacked(new_pos, !new_pos.turn));
                if (new_moves.size() == 0) {
                    continue;  // Continue if move results in stalemate.
                }
                const Location to(move.to);
                const char dx = abs(to.x-ok.x);
                const char dy = abs(to.y-ok.y);
                if ((dx==0 || dx==1) && (dy==0 || dy==1)) continue;  // If move goes to opponent's king
                if (dx == 0 || dy == 0 || dx == dy) continue;  // Don't check king
                const char dist = dx + dy;
                const char move_dist = std::max(abs(to.x-cq.x), abs(to.y-cq.y));
                if (dist < best_dist) {
                    best_move = move;
                    best_dist = dist;
                    if (move_dist < least_move_dist) least_move_dist = move_dist;
                } else if (dist <= best_dist && move_dist < least_move_dist) {
                    least_move_dist = move_dist;
                    best_move = move;
                }
            }
        }
        return best_move;
    }
}
