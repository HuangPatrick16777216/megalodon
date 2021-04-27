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
#include <algorithm>
#include "bitboard.hpp"
#include "search.hpp"
#include "eval.hpp"
#include "options.hpp"
#include "utils.hpp"
#include "hash.hpp"
#include "endgame.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


SearchInfo::SearchInfo() {
}

SearchInfo::SearchInfo(const int& _depth, const int& _seldepth, const float& _score, const U64& _nodes, const int& _nps,
        const int& _hashfull, const double& _time, const vector<Move>& _pv, const float& _alpha, const float& _beta,
        const bool& _full) {
    depth = _depth;
    seldepth = _seldepth;
    score = _score;
    nodes = _nodes;
    nps = _nps;
    hashfull = _hashfull;
    time = _time;
    pv = _pv;
    alpha = _alpha;
    beta = _beta;
    full = _full;
}

string SearchInfo::as_string() {
    string str = "";
    str += "info depth " + std::to_string(depth) + " seldepth " + std::to_string(seldepth);
    str += " multipv 1 score ";
    if (is_mate()) {
        str += "mate ";
        char moves;
        if (score < 0) moves = (score-Search::MIN+1) / 2;
        else           moves = (Search::MAX-score+1) / 2;
        str += std::to_string(moves);
    } else {
        str += "cp ";
        str += std::to_string((int)(100*score));
    }
    str += " nodes " + std::to_string(nodes) + " nps " + std::to_string(nps);
    str += " hashfull " + std::to_string(hashfull);
    str += " tbhits 0 time " + std::to_string((int)(1000*time));
    str += " pv ";
    for (const auto& move: pv) str += Bitboard::move_str(move) + " ";
    return str;
}

bool SearchInfo::is_mate() {
    return !(Search::MATE_BOUND_MIN <= score && score <= Search::MATE_BOUND_MAX);
}


namespace Search {
    float move_time(const Position& pos, const float& time, const float& inc) {
        const int moves = std::max(55-pos.move_cnt, 5);
        const float time_left = time + inc*moves;
        const float move_time = time_left / moves;
        return std::min(move_time, time/2);
    }


    SearchInfo dfs(const Options& options, const Position& pos, const int& depth, const int& real_depth,
            float alpha, float beta, const bool& root, const double& endtime, bool& searching, U64& hash_filled) {
        const U64 o_attacks = Bitboard::attacked(pos, !pos.turn);
        vector<Move> moves = Bitboard::legal_moves(pos, o_attacks);

        if (depth == 0 || moves.empty()) {
            const float score = Eval::eval(options, pos, moves, real_depth, o_attacks);
            return SearchInfo(depth, depth, score, 1, 0, 0, 0, {}, alpha, beta, true);
        }

        // Parse and store best move
        const U64 idx = Hash::hash(pos) % options.hash_size;
        Transposition& entry = options.hash_table[idx];
        const Move best(entry.from&63, entry.to&63, entry.to&64, (entry.from&192)>>6);
        if (entry.depth > 0) moves.insert(moves.begin(), best);

        U64 nodes = 1;
        vector<Move> pv;
        int best_ind = 0;
        float best_eval = pos.turn ? MIN : MAX;
        bool full = true;
        int movecnt = 0;  // Need to use this bc will skip a repeated best move so i != movecnt
        for (unsigned int i = 0; i < moves.size(); i++) {
            if (depth >= 3) {
                if ((get_time() >= endtime) || !searching) {
                    full = false;
                    break;
                }
            }
            if ((i != 0) && (entry.depth > 0)) {  // Don't search best move twice
                const Move& curr = moves[i];
                if ((best.from == curr.from) && (best.to == curr.to) && (best.is_promo == curr.is_promo) &&
                        (best.promo == curr.promo)) continue;
            }
            movecnt++;

            const Position new_pos = Bitboard::push(pos, moves[i]);
            const SearchInfo result = dfs(options, new_pos, depth-1, real_depth+1, alpha, beta, false, endtime, searching, hash_filled);
            nodes += result.nodes;

            if (root && (depth >= 5)) {
                cout << "info depth " << depth << " currmove " << Bitboard::move_str(moves[i]) << " currmovenumber " << movecnt << endl;
            }

            if (pos.turn) {
                if (result.score > best_eval) {
                    best_ind = i;
                    best_eval = result.score;
                    pv = result.pv;
                }
                if (result.score > alpha) alpha = result.score;
                if (beta < alpha) break;
            } else {
                if (result.score < best_eval) {
                    best_ind = i;
                    best_eval = result.score;
                    pv = result.pv;
                }
                if (result.score < beta) beta = result.score;
                if (beta < alpha) break;
            }
        }
        pv.insert(pv.begin(), moves[best_ind]);

        if (depth > entry.depth) {
            if (entry.depth == 0) hash_filled++;

            const Move& best_move = moves[best_ind];
            entry.from = best_move.from + (best_move.promo<<6);
            entry.to = best_move.to + (best_move.is_promo<<6);
            entry.depth = depth;
        }

        return SearchInfo(depth, depth, best_eval, nodes, 0, 0, 0, pv, alpha, beta, full);
    }

    SearchInfo search(const Options& options, const Position& pos, const int& depth, const double& movetime,
            const bool& infinite, bool& searching) {
        const int eg = Endgame::eg_type(pos);
        const U64 o_attacks = Bitboard::attacked(pos, !pos.turn);
        const vector<Move> moves = Bitboard::legal_moves(pos, o_attacks);
        if (false && (moves.size() == 1)) {
            return SearchInfo(1, 1, 0, 1, 1, 0, 0, {moves[0]}, 0, 0, true);
        }
        if (false && (eg != 0)) {
            const Move best_move = Endgame::bestmove(pos, moves, eg);
            return SearchInfo(1, 1, pos.turn ? MAX : MIN, moves.size(), 0, 0, 0, {best_move}, 0, 0, true);
        }

        SearchInfo result;
        U64 hash_filled = 0;
        U64 nodes = 0;
        const double start = get_time();
        const double end = start + movetime;

        for (char d = 1; d <= depth; d++) {
            if (!searching || get_time() >= end) break;

            SearchInfo curr_result = dfs(options, pos, d, 0, MIN, MAX, true, end, searching, hash_filled);
            const double elapse = get_time() - start;
            nodes += curr_result.nodes;

            curr_result.time = elapse;
            curr_result.nodes = nodes;
            curr_result.nps = curr_result.nodes / (elapse+0.001);
            curr_result.hashfull = 1000 * hash_filled / options.hash_size;
            if (!pos.turn) curr_result.score *= -1;
            if (curr_result.full) {
                cout << curr_result.as_string() << endl;
                result = curr_result;
            }
            if (curr_result.is_mate() && (curr_result.score > 0) && !infinite) break;

            // if (stop_early && ((elapse/movetime) >= 0.6)) {    // Won't finish next depth so no point
            //     break;
            // }
        }

        return result;
    }
}
