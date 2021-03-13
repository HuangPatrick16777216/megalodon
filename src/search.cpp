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
#include <climits>
#include "bitboard.hpp"
#include "utils.hpp"
#include "search.hpp"
#include "eval.hpp"
#include "options.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


SearchInfo::SearchInfo() {
}

SearchInfo::SearchInfo(int _depth, int _seldepth, bool _is_mate, int _score, int _nodes, int _nps, int _time, Move _move) {
    depth = _depth;
    seldepth = _seldepth;
    is_mate_score = _is_mate;
    score = _score;
    nodes = _nodes;
    nps = _nps;
    time = _time;
    move = _move;
}

string SearchInfo::as_string() {
    string str = "";
    str += "info depth " + std::to_string(depth) + " seldepth " + std::to_string(seldepth);
    str += " multipv 1 score ";
    str += (is_mate_score ? "mate" : "cp");
    str += " " + std::to_string((is_mate_score ? (int)score : (int)(100*score)));
    str += " nodes " + std::to_string(nodes) + " nps " + std::to_string(nps);
    str += " tbhits 0 time " + std::to_string(time);
    str += " pv";
    return str;
}


SearchInfo search(Options& options, Position pos, int total_depth) {
    pos.eval = eval(options, pos, false);
    vector<vector<vector<Position>>> tree = {{{pos}}};
    int depth = 1;
    int num_nodes = 1;

    while (true) {
        vector<vector<Position>> curr_depth;

        for (auto node: flatten(tree[depth-1])) {
            vector<Position> group;
            U64 attacks = Bitboard::attacked(pos, !pos.turn);

            for (auto move: Bitboard::legal_moves(pos, attacks)) {
                Position new_pos = Bitboard::push(node, move);
                if (new_pos.turn) new_pos.eval = eval(options, new_pos, true, Bitboard::attacked(pos, true), attacks);
                else new_pos.eval = eval(options, new_pos, true, attacks, Bitboard::attacked(pos, false));
                group.push_back(new_pos);
            }

            curr_depth.push_back(group);
            num_nodes += group.size();
        }
        tree.push_back(curr_depth);

        depth++;
        if (depth == total_depth) break;
    }

    return SearchInfo(depth, depth, false, 0, num_nodes, 0, 0, Move());
}
