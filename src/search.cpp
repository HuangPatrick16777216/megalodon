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
#include "search.hpp"
#include "eval.hpp"
#include "options.hpp"
#include "utils.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


SearchInfo::SearchInfo() {
}

SearchInfo::SearchInfo(int _depth, int _seldepth, bool _is_mate, float _score, int _nodes, int _nps,
        double _time, Move _move, float _alpha, float _beta) {
    depth = _depth;
    seldepth = _seldepth;
    is_mate_score = _is_mate;
    score = _score;
    nodes = _nodes;
    nps = _nps;
    time = _time;
    move = _move;
    alpha = _alpha;
    beta = _beta;
}

string SearchInfo::as_string() {
    string str = "";
    str += "info depth " + std::to_string(depth) + " seldepth " + std::to_string(seldepth);
    str += " multipv 1 score ";
    str += (is_mate_score ? "mate" : "cp");
    str += " " + std::to_string((is_mate_score ? (int)score : (int)(100*score)));
    str += " nodes " + std::to_string(nodes) + " nps " + std::to_string(nps);
    str += " tbhits 0 time " + std::to_string((int)(1000*time));
    str += " pv " + Bitboard::move_str(move);
    return str;
}


float moves_left(const Options& options, const Position& pos) {
    // Estimates moves left based on material and move stack.
    //float mat_left = 0.9 * (total_mat(pos)-7);
    float abs_left = 55 - pos.move_stack.size();
    if (abs_left < 5) abs_left = 5;
    //if (mat_left < 0) mat_left = 0;

    //float final_left = (mat_left+abs_left) / 2;
    //if (final_left < 1) final_left = 1;

    return abs_left;
}

float move_time(const Options& options, const Position& pos, const float& time, const float& inc) {
    // Calculates move time based on moves left and game evaluation.
    float moves = moves_left(options, pos);
    float time_left = time + inc*moves;
    return time_left / moves;
    //float mat = eval(options, pos, false);
    //if (!pos.turn) mat *= -1;
    //float mat_offset = mat * -0.3;

    //return (time_left/moves) + mat_offset;
}


SearchInfo dfs(const Options& options, const Position& pos, const int& depth, float alpha, float beta) {
    U64 o_attacks = Bitboard::attacked(pos, !pos.turn);
    vector<Move> moves = Bitboard::legal_moves(pos, o_attacks);

    if (depth == 0 || moves.size() == 0) {
        const float score = eval(options, pos, moves.size()!=0, depth, o_attacks);
        return SearchInfo(depth, depth, false, score, 1, 0, 0, Move(), alpha, beta);
    }
    int nodes = 1;
    int best_ind = 0;
    float best_eval = pos.turn ? MIN : MAX;

    for (auto i = 0; i < moves.size(); i++) {
        Position new_pos = Bitboard::push(pos, moves[i]);
        SearchInfo result = dfs(options, new_pos, depth-1, alpha, beta);
        nodes += result.nodes;

        if (pos.turn) {
            if (result.score > best_eval) {
                best_ind = i;
                best_eval = result.score;
            }
            if (result.score > alpha) alpha = result.score;
            if (beta <= alpha) break;
        } else {
            if (result.score < best_eval) {
                best_ind = i;
                best_eval = result.score;
            }
            if (result.score < beta) beta = result.score;
            if (beta <= alpha) break;
        }
    }
    return SearchInfo(depth, depth, false, best_eval, nodes, 0, 0, moves[best_ind], alpha, beta);
}

SearchInfo search(const Options& options, Position pos, const int& depth) {
    // Iterative deepening doesn't have any improvements yet.
    SearchInfo result;
    double start = get_time();

    for (auto d = 1; d <= depth; d++) {
        result = dfs(options, pos, d, MIN, MAX);
        double elapse = get_time() - start;

        result.time = elapse;
        result.nps = result.nodes / elapse;
        cout << result.as_string() << endl;
    }

    return result;
}
