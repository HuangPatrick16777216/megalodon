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
#include "hash.hpp"
#include "bitboard.hpp"
#include "utils.hpp"

#define SETS  255

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

using Bitboard::bit;


namespace Hash {
    U64 piece_bits[SETS][64][12];
    U64 ep_square[64];
    U64 turn[2];
    U64 ep[2];
    U64 castling[15];

    void init() {
        for (auto i = 0; i < SETS; i++) {
            for (auto j = 0; j < 64; j++) {
                for (auto k = 0; k < 12; k++) {
                    piece_bits[i][j][k] = randull();
                }
            }
        }
        for (auto i = 0; i < 64; i++) {
            ep_square[i] = randull();
        }
        for (auto i = 0; i < 15; i++) {
            castling[i] = randull();
        }
        turn[0] = randull();
        turn[1] = randull();
        ep[0] = randull();
        ep[1] = randull();
    }

    U64 hash(const Position& pos) {
        const U64 kings = pos.wk | pos.bk;
        const char idx = kings % SETS;
        U64 value = 0;
        for (auto i = 0; i < 64; i++) {
            if      (bit(pos.wp, i)) value ^= piece_bits[idx][i][0];
            else if (bit(pos.wn, i)) value ^= piece_bits[idx][i][1];
            else if (bit(pos.wb, i)) value ^= piece_bits[idx][i][2];
            else if (bit(pos.wr, i)) value ^= piece_bits[idx][i][3];
            else if (bit(pos.wq, i)) value ^= piece_bits[idx][i][4];
            else if (bit(pos.wk, i)) value ^= piece_bits[idx][i][5];
            else if (bit(pos.bp, i)) value ^= piece_bits[idx][i][6];
            else if (bit(pos.bn, i)) value ^= piece_bits[idx][i][7];
            else if (bit(pos.bb, i)) value ^= piece_bits[idx][i][8];
            else if (bit(pos.br, i)) value ^= piece_bits[idx][i][9];
            else if (bit(pos.bq, i)) value ^= piece_bits[idx][i][10];
            else if (bit(pos.bk, i)) value ^= piece_bits[idx][i][11];
        }
        value ^= turn[pos.turn];
        value ^= ep[pos.ep];
        value ^= castling[pos.castling];
        value ^= ep_square[pos.ep_square];
        return value;
    }
}
