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
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "consts.hpp"

struct Move {
    Move();
    Move(const char, const char, const bool=false, const char=0);
    UCH from;
    UCH to;
    UCH promo;  // 0, 1, 2, 3
    bool is_promo;
};
struct Position {
    Position();
    Position(const U64, const U64, const U64, const U64, const U64, const U64, const U64, const U64, const U64,
        const U64, const U64, const U64, const bool, const char, const bool, const char);
    U64 wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk;
    bool turn;
    UCH castling;
    UCH ep_square;
    bool ep;
    Position* parent;
    float eval;
    int move_cnt;
    UCH draw50;
};
struct Location {
    Location();
    Location(const UCH, const UCH);
    Location(const UCH);
    UCH x;
    UCH y;
    UCH loc;
};
namespace Bitboard {
    // Promotion piece format: 0=knight, 1=bishop, 2=rook, 3=queen
    constexpr U64 EMPTY = 0ULL;
    constexpr U64 FULL = 18446744073709551615ULL;
    constexpr U64 START_WP = 65280ULL;
    constexpr U64 START_WN = 66ULL;
    constexpr U64 START_WB = 36ULL;
    constexpr U64 START_WR = 129ULL;
    constexpr U64 START_WQ = 8ULL;
    constexpr U64 START_WK = 16ULL;
    constexpr U64 START_BP = 71776119061217280ULL;
    constexpr U64 START_BN = 4755801206503243776ULL;
    constexpr U64 START_BB = 2594073385365405696ULL;
    constexpr U64 START_BR = 9295429630892703744ULL;
    constexpr U64 START_BQ = 576460752303423488ULL;
    constexpr U64 START_BK = 1152921504606846976ULL;
    constexpr U64 CASTLING_BK = 8070450532247928832ULL;
    constexpr U64 CASTLING_BQ = 2017612633061982208ULL;
    constexpr U64 CASTLING_WQ = 28ULL;
    constexpr U64 CASTLING_WK = 112ULL;
    constexpr U64 RANK1 = 255ULL;
    constexpr U64 RANK2 = 65280ULL;
    constexpr U64 RANK3 = 16711680ULL;
    constexpr U64 RANK4 = 4278190080ULL;
    constexpr U64 RANK5 = 1095216660480ULL;
    constexpr U64 RANK6 = 280375465082880ULL;
    constexpr U64 RANK7 = 71776119061217280ULL;
    constexpr U64 RANK8 = 18374686479671623680ULL;
    constexpr U64 FILE1 = 72340172838076673ULL;
    constexpr U64 FILE2 = 144680345676153346ULL;
    constexpr U64 FILE3 = 289360691352306692ULL;
    constexpr U64 FILE4 = 578721382704613384ULL;
    constexpr U64 FILE5 = 1157442765409226768ULL;
    constexpr U64 FILE6 = 2314885530818453536ULL;
    constexpr U64 FILE7 = 4629771061636907072ULL;
    constexpr U64 FILE8 = 9259542123273814144ULL;
    constexpr U64 RANKS[8] = {RANK1, RANK2, RANK3, RANK4, RANK5, RANK6, RANK7, RANK8};
    constexpr U64 FILES[8] = {FILE1, FILE2, FILE3, FILE4, FILE5, FILE6, FILE7, FILE8};
    constexpr U64 BYTE_ALL_ONE = 255ULL;
    constexpr int MAX_MOVES = 220;
    constexpr int MAX_HASH_MOVES = 30;
    constexpr char DIR_R_SIZE = 4;
    constexpr char DIR_N_SIZE = 8;
    constexpr char DIR_B_SIZE = 4;
    constexpr char DIR_Q_SIZE = 8;
    constexpr char DIR_K_SIZE = 8;
    constexpr char DIR_R[DIR_R_SIZE][2] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};
    constexpr char DIR_N[DIR_N_SIZE][2] = {{-2, 1}, {2, 1}, {-2, -1}, {2, -1}, {1, -2}, {-1, 2}, {-1, -2}, {1, 2}};
    constexpr char DIR_B[DIR_B_SIZE][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
    constexpr char DIR_Q[DIR_Q_SIZE][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {0, 1}, {-1, 0}, {1, 0}, {0, -1}};
    constexpr char DIR_K[DIR_K_SIZE][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    bool bit(const U64&, const char&);
    bool bit(const UCH&, const char&);
    char popcnt(const U64&);
    void set_bit(U64&, const char&);
    void unset_bit(U64&, const char&);
    void set_bit(UCH&, const char&);
    void unset_bit(UCH&, const char&);
    bool in_board(const char&, const char&);
    char first_bit_char(const U64&);
    Location first_bit(const U64&);
    string piece_at(const Position&, const char&);
    U64 color(const Position&, const bool&);
    string board_str(const U64&, const string="X", const string="-");
    string board_str(const Position&);
    string square_str(const char&);
    string move_str(const Move&);
    string fen(const Position&);
    Position parse_fen(const string&);
    Move parse_uci(const string&);
    U64 attacked(const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const bool&);
    U64 attacked(const Position&, const bool&);
    char num_attacks(const vector<Move>&, const Location&);
    U64 pinned(const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&);
    U64 checkers(const Location&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const bool&);
    void king_moves(Move*, int&, const Location&, const UCH&, const bool&, const U64&, const U64&, const U64&);
    void single_check_moves(Move*, int&, const Position&, const U64&, const U64&, const U64&, const U64&,
        const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&,
        const U64&, const U64&, const U64&, const Location&, const U64&);
    void no_check_moves(Move*, int&, const Position&, const U64&, const U64&, const U64&, const U64&,
        const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&, const U64&,
        const U64&, const U64&, const U64&, const Location&, const U64&);
    vector<Move> legal_moves(Position, const U64&);
    U64 get_white(const Position&);
    U64 get_black(const Position&);
    U64 get_all(const Position&);
    Position startpos();
    Position push(Position, const Move&);
    Position push(Position, const string&);
}
