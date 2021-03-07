//
//  Megalodon
//  UCI chess engine
//  Copyright Patrick Huang and Arjun Sahlot 2021
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
#include <fstream>
#include <vector>
#include <string>
#include "chess/board.hpp"
#include "chess/funcs.hpp"
#include "eval.hpp"
#include "options.hpp"

using std::cin;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;


float material(Board board) {
    float mat = 0;

    for (auto row: board.board()) {
        for (auto piece: row) {
            int value = piece_value(piece);
            mat += (piece >= 11) ? -value : value;
        }
    }

    return mat;
}

float material_weight(int movect) {
    return 1.0;
}


float piece_map(Board board, Options& options) {
    if (!options.pm_loaded) {
        options.pm_pawn.set_weights("pmaps/pawn");
        options.pm_knight.set_weights("pmaps/knight");
        options.pm_bishop.set_weights("pmaps/bishop");
        options.pm_rook.set_weights("pmaps/rook");
        options.pm_queen.set_weights("pmaps/queen");
        options.pm_king.set_weights("pmaps/king");

        options.pm_pawn.normalize(0.2);
        options.pm_knight.normalize(0.5);
        options.pm_bishop.normalize(0.3);
        options.pm_rook.normalize(0.4);
        options.pm_queen.normalize(0.5);
        options.pm_king.normalize(0.3);

        options.pm_loaded = true;
    }

    float pawns = options.pm_pawn.eval(board, WP) - options.pm_pawn.eval(board, BP);
    float knights = options.pm_knight.eval(board, WN) - options.pm_knight.eval(board, BN);
    float bishops = options.pm_bishop.eval(board, WB) - options.pm_bishop.eval(board, BB);
    float rooks = options.pm_rook.eval(board, WR) - options.pm_rook.eval(board, BR);
    float queens = options.pm_queen.eval(board, WQ) - options.pm_queen.eval(board, BQ);
    float kings = options.pm_king.eval(board, WK) - options.pm_king.eval(board, BK);

    return pawns + knights + bishops + rooks + queens + kings;
}

float piece_map_weight(int movect) {
    float weight = -0.05*movect + 2;
    if (weight < 0) weight = 0;
    return weight;
}


float center(Board board) {
    int count = 0;
    float points = 0.0;
    vector<vector<int>> inner_center = {{3, 3}, {3, 4}, {4, 3}, {4, 4}};

    for (auto sq: inner_center) {
        vector<int> w_attack = board.attackers(sq, true);
        vector<int> b_attack = board.attackers(sq, false);
        for (auto p: w_attack) {
            switch (p) {
                case WP: points += 2.5;
                case WN: points += 1.75;
                case WB: points += 1.6;
                case WR: points += 1.3;
                case WQ: points += 1.3;
                case WK: points += 1;
            }
            count++;
        }
        for (auto p: b_attack) {
            switch (p) {
                case BP: points -= 2.5;
                case BN: points -= 1.75;
                case BB: points -= 1.6;
                case BR: points -= 1.3;
                case BQ: points -= 1.3;
                case BK: points -= 1;
            }
            count++;
        }
    }
    return points / count;
}

float center_control(int movect) {
    return 1.0;
}


float eval(Board board, Options& options) {
    int movect = board.move_stack().size();

    float mat = material(board) * material_weight(movect);
    float pmaps;
    if (options.UsePieceMaps) pmaps = piece_map(board, options) * piece_map_weight(movect);
    else pmaps = 0;

    return mat + pmaps;
}
