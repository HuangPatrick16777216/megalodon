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
#include <vector>
#include <string>
#include "bitboard.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


Move::Move() {
}

Move::Move(char _from, char _to, bool _is_promo, char _promo) {
    from = _from;
    to = _to;
    is_promo = _is_promo;
    promo = _promo;
}


Position::Position() {
}

Position::Position(U64 _wp, U64 _wn, U64 _wb, U64 _wr, U64 _wq, U64 _wk, U64 _bp, U64 _bn, U64 _bb, U64 _br, U64 _bq, U64 _bk,
        bool _turn, char _castling, bool _ep, char _ep_square) {
    wp = _wp;
    wn = _wn;
    wb = _wb;
    wr = _wr;
    wq = _wq;
    wk = _wk;
    bp = _bp;
    bn = _bn;
    bb = _bb;
    br = _br;
    bq = _bq;
    bk = _bk;

    turn = _turn;
    castling = _castling;
    ep = _ep;
    ep_square = _ep_square;
}


namespace Bitboard {
    bool bit(U64 board, int pos) {
        return ((1LL << pos) & board) != 0;
    }

    bool bit(char board, int pos) {
        return ((1 << pos) & board) != 0;
    }

    U64 set_bit(U64 board, int pos, bool value) {
        bool on = bit(board, pos);
        if (value && !on) {
            board += (1LL << pos);
        } else if (!value && on) {
            board -= (1LL << pos);
        }
        return board;
    }

    string board_str(U64 board, string on, string off) {
        vector<string> rows;
        string repr = "";

        for (auto y = 0; y < 8; y++) {
            string row = "";
            for (auto x = 0; x < 8; x++) {
                row += bit(board, 8*y+x) ? on : off;
                row += " ";
            }
            rows.push_back(row);
        }

        for (auto i = 7; i >= 0; i--) {
            repr += rows[i];
            repr += "\n";
        }

        return repr;
    }

    string move_str(Move move) {
        string str;
        for (auto sq: {move.from, move.to}) {
            char x = sq%8, y = sq/8;
            str += string(1, x+97);
            str += std::to_string(y+1);
        }
        return str;
    }

    U64 attacked(U64 pawns, U64 knights, U64 bishops, U64 rooks, U64 queens, U64 kings, U64 opponent, bool side) {
        U64 pieces = pawns | knights | bishops | rooks | queens | kings;
        U64 board = EMPTY;
        char pawn_dir = side ? 1 : -1;

        for (char i = 0; i < 64; i++) {
            if (bit(pawns, i)) {
                char x = i%8, y = i/8 + pawn_dir;  // Current (x, y) with y as after capture.
                if (0 <= y && y < 8) {
                    if (0 <= x-1 && x-1 < 8) board = set_bit(board, y*8 + x-1, true);
                    if (0 <= x+1 && x+1 < 8) board = set_bit(board, y*8 + x+1, true);
                }
            }

            if (bit(knights, i)) {
                char x = i%8, y = i/8;
                for (auto dir: DIR_N) {                 // Iterate through all knight moves.
                    char nx = x+dir[0], ny = y+dir[1];   // Position after moving.
                    if (0 <= nx && nx < 8 && 0 <= ny && ny < 8) board = set_bit(board, ny*8 + nx, true);
                }
            }

            if (bit(kings, i)) {
                char x = i%8, y = i/8;
                for (auto dir: DIR_K) {
                    char nx = x+dir[0], ny = y+dir[1];
                    if (0 <= nx && nx < 8 && 0 <= ny && ny < 8) board = set_bit(board, ny*8 + nx, true);
                }
            }

            if (bit(rooks, i) || bit(queens, i)) {
                char x = i%8, y = i/8;
                for (auto dir: DIR_R) {
                    char cx = x, cy = y;            // Current (x, y)
                    char dx = dir[0], dy = dir[1];  // Delta (x, y)
                    while (true) {
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        char loc = cy*8 + cx;
                        board = set_bit(board, loc, true);
                        if (bit(opponent, loc)) break;
                        if (bit(pieces, loc)) break;
                    }
                }
            }

            if (bit(bishops, i) || bit(queens, i)) {
                char x = i%8, y = i/8;
                for (auto dir: DIR_B) {
                    char cx = x, cy = y;            // Current (x, y)
                    char dx = dir[0], dy = dir[1];  // Delta (x, y)
                    while (true) {
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        char loc = cy*8 + cx;
                        board = set_bit(board, loc, true);
                        if (bit(opponent, loc)) break;
                        if (bit(pieces, loc)) break;
                    }
                }
            }
        }

        return board;
    }

    vector<Move> king_moves(U64 king, U64 attacks) {
        vector<Move> moves;

        for (char i = 0; i < 64; i++) {
            if (bit(king, i)) {
                char x = i%8, y = i/8;
                for (auto dir: DIR_K) {
                    char nx = x+dir[0], ny = y+dir[1];
                    if (0 <= nx && nx < 8 && 0 <= ny && ny < 8) {
                        char new_loc = ny*8 + nx;
                        if (((1ULL << new_loc) & attacks) == 0) moves.push_back(Move(i, new_loc));
                    }
                }
                break;
            }
        }

        return moves;
    }
}
