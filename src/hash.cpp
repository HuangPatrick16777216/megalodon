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
#include "consts.hpp"
#include "utils.hpp"


namespace Hash {
    U64 piece_bits[101][64][12];
    U64 ep_square[64];
    U64 turn[2];
    U64 ep[2];
    U64 castling[15];
    void init() {
        for (UCH i = 0; i < 101; i++) {
            for (UCH j = 0; j < 64; j++) {
                for (UCH k = 0; k < 12; k++) {
                    piece_bits[i][j][k] = randull();
                }
            }
        }
        for (UCH i = 0; i < 64; i++) {
            ep_square[i] = randull();
        }
        for (UCH i = 0; i < 15; i++) {
            castling[i] = randull();
        }
        turn[0] = randull();
        turn[1] = randull();
        ep[0] = randull();
        ep[1] = randull();
    }
    U64 hash(const Position& pos) {
        const U64 kings = pos.wk | pos.bk;
        const UCH idx = kings % 101;
        U64 value = 0;
        for (UCH i = 0; i < 64; i++) {
            if      (Bitboard::bit(pos.wp, i)) value ^= piece_bits[idx][i][0];
            else if (Bitboard::bit(pos.wn, i)) value ^= piece_bits[idx][i][1];
            else if (Bitboard::bit(pos.wb, i)) value ^= piece_bits[idx][i][2];
            else if (Bitboard::bit(pos.wr, i)) value ^= piece_bits[idx][i][3];
            else if (Bitboard::bit(pos.wq, i)) value ^= piece_bits[idx][i][4];
            else if (Bitboard::bit(pos.wk, i)) value ^= piece_bits[idx][i][5];
            else if (Bitboard::bit(pos.bp, i)) value ^= piece_bits[idx][i][6];
            else if (Bitboard::bit(pos.bn, i)) value ^= piece_bits[idx][i][7];
            else if (Bitboard::bit(pos.bb, i)) value ^= piece_bits[idx][i][8];
            else if (Bitboard::bit(pos.br, i)) value ^= piece_bits[idx][i][9];
            else if (Bitboard::bit(pos.bq, i)) value ^= piece_bits[idx][i][10];
            else if (Bitboard::bit(pos.bk, i)) value ^= piece_bits[idx][i][11];
        }
        value ^= turn[pos.turn];
        value ^= ep[pos.ep];
        value ^= castling[pos.castling];
        value ^= ep_square[pos.ep_square];
        return value;
    }
}
