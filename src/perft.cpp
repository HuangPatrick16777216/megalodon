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
#include "utils.hpp"
#include "hash.hpp"
#include "eval.hpp"


namespace Perft {
    long long movegen(const Position& pos, const int& depth) {
        if (depth == 0) return 1;
        long long count = 0;
        for (const auto& move: Bitboard::legal_moves(pos, Bitboard::attacked(pos, !pos.turn))) {
            const Position new_pos = Bitboard::push(pos, move);
            count += movegen(new_pos, depth-1);
        }
        return count;
    }
    double hash_perft(const Position& pos, const int& knodes) {
        const double start = get_time();
        for (auto i = 0; i < knodes*1000; i++) Hash::hash(pos);
        return get_time() - start;
    }
    double eval_perft(const Options& options, const Position& pos, const int& knodes) {
        const U64 o_attacks = Bitboard::attacked(pos, !pos.turn);
        const vector<Move> moves = Bitboard::legal_moves(pos, o_attacks);
        const double start = get_time();
        for (auto i = 0; i < knodes*1000; i++) Eval::eval(options, pos, moves, 0, o_attacks);
        return get_time() - start;
    }
    double push_perft(const Position& pos, const int& knodes) {
        const Move move = Bitboard::legal_moves(pos, Bitboard::attacked(pos, !pos.turn))[0];
        const double start = get_time();
        for (auto i = 0; i < knodes*1000; i++) Bitboard::push(pos, move);
        return get_time() - start;
    }
}
