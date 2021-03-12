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
#include <cmath>
#include <algorithm>
#include <utility>
#include "bitboard.hpp"
#include "utils.hpp"

using std::abs;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
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
    wp = Bitboard::EMPTY;
    wn = Bitboard::EMPTY;
    wb = Bitboard::EMPTY;
    wr = Bitboard::EMPTY;
    wq = Bitboard::EMPTY;
    wk = Bitboard::EMPTY;
    bp = Bitboard::EMPTY;
    bn = Bitboard::EMPTY;
    bb = Bitboard::EMPTY;
    br = Bitboard::EMPTY;
    bq = Bitboard::EMPTY;
    bk = Bitboard::EMPTY;

    turn = true;
    castling = 0;
    ep = false;
    ep_square = 0;
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


Position copy(Position p) {
    return Position(p.wp, p.wn, p.wb, p.wr, p.wq, p.wk, p.bp, p.bn, p.bb, p.br, p.bq, p.bk, p.turn, p.castling, p.ep, p.ep_square);
}

Move copy(Move m) {
    return Move(m.from, m.to, m.is_promo, m.promo);
}


namespace Bitboard {
    bool bit(U64 board, int pos) {
        return ((1LL << pos) & board) != 0;
    }

    bool bit(char board, int pos) {
        return ((1 << pos) & board) != 0;
    }

    char bit_pos(U64 board, bool target) {
        for (char i = 0; i < 64; i++) {
            if (bit(board, i) == target) return i;
        }
        return 64;
    }

    char bit_count(U64 board, bool target) {
        char count = 0;
        for (auto i = 0; i < 64; i++) {
            if (bit(board, i) == target) count++;
        }
        return count;
    }

    U64 set_bit(U64 board, int pos, bool value) {
        const bool on = bit(board, pos);
        if (value && !on) {
            board += (1LL << pos);
        } else if (!value && on) {
            board -= (1LL << pos);
        }
        return board;
    }

    pair<char, char> first_bit(U64 board) {
        for (char i = 0; i < 64; i++) {
            if (bit(board, i)) {
                const char x = i % 8, y = i / 8;
                return pair<char, char>(x, y);
            }
        }
        return pair<char, char>(0, 8);
    }


    string piece_at(Position pos, char loc) {
        if (bit(pos.wp, loc)) return "P";
        else if (bit(pos.wn, loc)) return "N";
        else if (bit(pos.wb, loc)) return "B";
        else if (bit(pos.wr, loc)) return "R";
        else if (bit(pos.wq, loc)) return "Q";
        else if (bit(pos.wk, loc)) return "K";
        else if (bit(pos.bp, loc)) return "p";
        else if (bit(pos.bn, loc)) return "n";
        else if (bit(pos.bb, loc)) return "b";
        else if (bit(pos.br, loc)) return "r";
        else if (bit(pos.bq, loc)) return "q";
        else if (bit(pos.bk, loc)) return "k";
        else return " ";
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

    string board_str(Position pos) {
        const string row = " +---+---+---+---+---+---+---+---+";
        const string col = " | ";
        string str = "";

        str += row + "\n";
        for (auto y = 7; y >= 0; y--) {
            for (auto x = 0; x < 8; x++) {
                int loc = 8*y + x;
                string symbol = piece_at(pos, loc);
                str += col;
                str += symbol;
            }
            str += col + "\n" + row + "\n";
        }
        str += "\nFen: " + fen(pos) + "\n";

        return str;
    }

    string square_str(char sq) {
        char x = sq%8, y = sq/8;
        return string(1, x+97) + std::to_string(y+1);
    }

    string move_str(Move move) {
        return square_str(move.from) + square_str(move.to);
    }

    string fen(Position pos) {
        string str = "";
        int blank_count = 0;

        for (auto y = 7; y >= 0; y--) {
            for (auto x = 0; x < 8; x++) {
                string piece = piece_at(pos, 8*y+x);
                if (piece == " ") {
                    blank_count++;
                } else {
                    if (blank_count > 0) {
                        str += std::to_string(blank_count);
                        blank_count = 0;
                    }
                    str += piece;
                }
            }
            if (blank_count > 0) str += std::to_string(blank_count);
            if (y != 0) str += "/";
            blank_count = 0;
        }

        str += (pos.turn ? " w " : " b ");

        vector<bool> c = {bit(pos.castling, 0), bit(pos.castling, 1), bit(pos.castling, 2), bit(pos.castling, 3)};
        if (!(c[0] || c[1] || c[2] || c[3])) str += "-";
        else {
            if (c[0]) str += "K";
            if (c[1]) str += "Q";
            if (c[2]) str += "k";
            if (c[3]) str += "q";
        }
        str += " ";

        if (pos.ep) {
            str += square_str(pos.ep_square);
        } else {
            str += "-";
        }
        str += " ";

        str += "0 1";

        return str;
    }

    Position parse_fen(string fen) {
        vector<string> parts = split(fen, " ");
        Position pos;

        int x = 0, y = 7;
        for (auto ch: parts[0]) {
            if (ch == '/') {
                y--;
                x = 0;
            } else {
                int loc = 8*y + x;
                if (48 <= ch && ch <= 57) {
                    x += (ch-48);
                } else {
                    switch (ch) {
                        case 'P': pos.wp = set_bit(pos.wp, loc, true); break;
                        case 'N': pos.wn = set_bit(pos.wn, loc, true); break;
                        case 'B': pos.wb = set_bit(pos.wb, loc, true); break;
                        case 'R': pos.wr = set_bit(pos.wr, loc, true); break;
                        case 'Q': pos.wq = set_bit(pos.wq, loc, true); break;
                        case 'K': pos.wk = set_bit(pos.wk, loc, true); break;
                        case 'p': pos.bp = set_bit(pos.bp, loc, true); break;
                        case 'n': pos.bn = set_bit(pos.bn, loc, true); break;
                        case 'b': pos.bb = set_bit(pos.bb, loc, true); break;
                        case 'r': pos.br = set_bit(pos.br, loc, true); break;
                        case 'q': pos.bq = set_bit(pos.bq, loc, true); break;
                        case 'k': pos.bk = set_bit(pos.bk, loc, true); break;
                    }
                }
                x++;
            }
        }

        pos.turn = (parts[1] == "w");

        int castling = 0;
        for (auto ch: parts[2]) {
            switch (ch) {
                case 'K': castling += 1; break;
                case 'Q': castling += 2; break;
                case 'k': castling += 4; break;
                case 'q': castling += 8; break;
            }
        }
        pos.castling = castling;

        if (parts[3] == "-") pos.ep = false;
        else {
            pos.ep = true;
            pos.ep_square = std::stoi(parts[3]);
        }

        return pos;
    }


    U64 attacked(U64 pawns, U64 knights, U64 bishops, U64 rooks, U64 queens, U64 kings, U64 opponent, bool side) {
        const U64 pieces = pawns | knights | bishops | rooks | queens | kings;
        U64 board = EMPTY;
        const char pawn_dir = side ? 1 : -1;

        for (char i = 0; i < 64; i++) {
            const char x = i % 8, y = i / 8;
            if (bit(pawns, i)) {
                const char y = y + pawn_dir;  // Current (x, y) with y as after capture.
                if (0 <= y && y < 8) {
                    if (0 <= x-1 && x-1 < 8) board = set_bit(board, y*8 + x-1, true);
                    if (0 <= x+1 && x+1 < 8) board = set_bit(board, y*8 + x+1, true);
                }
            }

            if (bit(knights, i)) {
                for (auto dir: DIR_N) {                        // Iterate through all knight moves.
                    const char nx = x+dir[0], ny = y+dir[1];   // Position after moving.
                    if (0 <= nx && nx < 8 && 0 <= ny && ny < 8) board = set_bit(board, ny*8 + nx, true);
                }
            }

            if (bit(kings, i)) {
                for (auto dir: DIR_K) {
                    const char kx = x+dir[0], ky = y+dir[1];
                    if (0 <= kx && kx < 8 && 0 <= ky && ky < 8) board = set_bit(board, ky*8 + kx, true);
                }
            }

            if (bit(rooks, i) || bit(queens, i)) {
                for (auto dir: DIR_R) {
                    char cx = x, cy = y;                  // Current (x, y)
                    const char dx = dir[0], dy = dir[1];  // Delta (x, y)
                    while (true) {
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        const char loc = cy*8 + cx;
                        board = set_bit(board, loc, true);
                        if (bit(opponent, loc)) break;
                        if (bit(pieces, loc)) break;
                    }
                }
            }

            if (bit(bishops, i) || bit(queens, i)) {
                for (auto dir: DIR_B) {
                    char cx = x, cy = y;                  // Current (x, y)
                    const char dx = dir[0], dy = dir[1];  // Delta (x, y)
                    while (true) {
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        const char loc = cy*8 + cx;
                        board = set_bit(board, loc, true);
                        if (bit(opponent, loc)) break;
                        if (bit(pieces, loc)) break;
                    }
                }
            }
        }

        return board;
    }

    U64 attacked(Position pos, bool turn) {
        if (turn) {
            U64 opp = pos.bp | pos.bn | pos.bb | pos.br | pos.bq | pos.bk;
            return attacked(pos.wp, pos.wn, pos.wb, pos.wr, pos.wq, pos.wk, opp, true);
        } else {
            U64 opp = pos.wp | pos.wn | pos.wb | pos.wr | pos.wq | pos.wk;
            return attacked(pos.bp, pos.bn, pos.bb, pos.br, pos.bq, pos.bk, opp, false);
        }
    }

    bool pinned(U64 king, U64 piece, U64 pawns, U64 knights, U64 bishops, U64 rooks, U64 queens, U64 same) {
        const U64 opposite = pawns | knights | bishops | rooks | queens;
        const pair<char, char> k_pos = first_bit(king);
        const char kx = k_pos.first, ky = k_pos.second;
        bool found;

        for (auto dir: DIR_R) {
            found = false;
            char cx = kx, cy = ky;                  // Current (x, y)
            const char dx = dir[0], dy = dir[1];    // Delta (x, y)
            while (true) {
                cx += dx;
                cy += dy;
                if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                const char loc = cy*8 + cx;
                if (bit(rooks, loc) || bit(queens, loc)) return found;
                if (bit(opposite, loc)) return false;
                if (bit(piece, loc)) found = true;
                if (bit(same, loc)) if (found) return true;
            }
            if (found) return false;
        }

        for (auto dir: DIR_B) {
            found = false;
            char cx = kx, cy = ky;                  // Current (x, y)
            const char dx = dir[0], dy = dir[1];    // Delta (x, y)
            while (true) {
                cx += dx;
                cy += dy;
                if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                const char loc = cy*8 + cx;
                if (bit(bishops, loc) || bit(queens, loc)) return found;
                if (bit(opposite, loc)) return false;
                if (bit(piece, loc)) found = true;
                if (bit(same, loc)) if (found) return true;
            }
            if (found) return false;
        }
        return false;
    }

    pair<U64, char> checkers(U64 king, U64 pawns, U64 knights, U64 bishops, U64 rooks, U64 queens, U64 same_side, bool side) {
        const U64 pieces = pawns | knights | bishops | rooks | queens;
        U64 board = EMPTY;
        char num_atckers = 0;
        const char pawn_dir = side ? 1 : -1;
        const pair<char, char> k_pos = first_bit(king);
        const char kx = k_pos.first, ky = k_pos.second;

        for (char i = 0; i < 64; i++) {
            if (bit(pawns, i)) {
                const char y = i/8 + pawn_dir;  // Current (x, y) with y as after capture.
                if (0 <= y && y < 8) {
                    if (num_atckers > 1) return pair<U64, char>(board, num_atckers);
                    if (0 <= kx-1 && kx-1 < 8) {
                        char pos = y*8 + kx-1;
                        if (bit(pawns, pos)) {
                            board = set_bit(board, pos, true);
                            num_atckers++;
                            continue;
                        }

                    }
                    if (num_atckers > 1) return pair<U64, char>(board, num_atckers);
                    if (0 <= kx+1 && kx+1 < 8) {
                        char pos = y*8 + kx+1;
                        if (bit(pawns, pos)) {
                            board = set_bit(board, pos, true);
                            num_atckers++;
                            continue;
                        }
                    }
                }
            }

            if (bit(knights, i)) {
                for (auto dir: DIR_N) {                          // Iterate through all knight moves.
                    if (num_atckers > 1) return pair<U64, char>(board, num_atckers);
                    const char nx = kx+dir[0], ny = ky+dir[1];   // Position after moving.
                    if (0 <= nx && nx < 8 && 0 <= ny && ny < 8) {
                        char pos = ny*8 + nx;   
                        if (bit(knights, pos)) {
                            board = set_bit(board, pos, true);
                            num_atckers++;
                            break;
                        }
                    }
                }
            }

            if (bit(rooks, i) || bit(queens, i)) {
                for (auto dir: DIR_R) {
                    char cx = kx, cy = ky;                  // Current (x, y)
                    const char dx = dir[0], dy = dir[1];    // Delta (x, y)
                    while (true) {
                        if (num_atckers > 1) return pair<U64, char>(board, num_atckers);
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        const char loc = cy*8 + cx;
                        if (bit(same_side, loc)) {
                            break;
                        } else if (bit(rooks, loc)) {
                            board = set_bit(board, loc, true);
                            num_atckers++;
                            break;
                        } else if (bit(queens, loc)) {
                            board = set_bit(board, loc, true);
                            num_atckers++;
                            break;
                        } else if (bit(pieces, loc)) {
                            break;
                        }
                    }
                }
            }

            if (bit(bishops, i) || bit(queens, i)) {
                for (auto dir: DIR_B) {
                    char cx = kx, cy = ky;                  // Current (x, y)
                    const char dx = dir[0], dy = dir[1];    // Delta (x, y)
                    while (true) {
                        if (num_atckers > 1) return pair<U64, char>(board, num_atckers);
                        cx += dx;
                        cy += dy;
                        if (!(0 <= cx && cx < 8 && 0 <= cy && cy < 8)) break;
                        const char loc = cy*8 + cx;
                        if (bit(same_side, loc)) {
                            break;
                        } else if (bit(bishops, loc)) {
                            board = set_bit(board, loc, true);
                            num_atckers++;
                            break;
                        } else if (bit(queens, loc)) {
                            board = set_bit(board, loc, true);
                            num_atckers++;
                            break;
                        } else if (bit(pieces, loc)) {
                            break;
                        }
                    }
                }
            }
        }
        return pair<U64, char>(board, num_atckers);
    }

    vector<Move> king_moves(U64 king, U64 attacks) {
        vector<Move> moves;

        for (char i = 0; i < 64; i++) {
            if (bit(king, i)) {
                const char x = i%8, y = i/8;
                for (auto dir: DIR_K) {
                    const char kx = x+dir[0], ky = y+dir[1];
                    if (0 <= kx && kx < 8 && 0 <= ky && ky < 8) {
                        const char new_loc = ky*8 + kx;
                        if (((1ULL << new_loc) & attacks) == 0) moves.push_back(Move(i, new_loc));
                    }
                }
                break;
            }
        }

        return moves;
    }

    vector<Move> legal_moves(Position pos, U64 attacks) {
        // Current and opposite pieces and sides
        U64 CP, CN, CB, CR, CQ, CK, OP, ON, OB, OR, OQ, OK, SAME, OPPOSITE;
        if (pos.turn) {
            CP = pos.wp;
            CN = pos.wn;
            CB = pos.wb;
            CR = pos.wr;
            CQ = pos.wq;
            CK = pos.wk;
            OP = pos.bp;
            ON = pos.bn;
            OB = pos.bb;
            OR = pos.br;
            OQ = pos.bq;
            OK = pos.bk;
        } else {
            CP = pos.bp;
            CN = pos.bn;
            CB = pos.bb;
            CR = pos.br;
            CQ = pos.bq;
            CK = pos.bk;
            OP = pos.wp;
            ON = pos.wn;
            OB = pos.wb;
            OR = pos.wr;
            OQ = pos.wq;
            OK = pos.wk;
        }
        SAME = CP | CN | CB | CR | CQ | CK;
        OPPOSITE = pos.wp | pos.wn | pos.wb | pos.wr | pos.wq | pos.wk;

        vector<Move> moves = king_moves(CK, attacks);
        pair<U64, char> checking_data = checkers(CK, OP, ON, OB, OR, OQ, SAME, pos.turn);
        U64 checking_pieces = checking_data.first;
        char num_checkers = checking_data.second;

        if (num_checkers > 1) {
            return moves;
        } else if (num_checkers == 1) {
            // Block and capture piece giving check to king
            U64 block_mask = EMPTY, capture_mask = checking_pieces;
            pair<char, char> k_pos = first_bit(CK), check_pos = first_bit(checking_pieces);
            const char kx = k_pos.first, ky = k_pos.second, cx = check_pos.first, cy = check_pos.second;

            char dx = abs(cx - kx), dy = abs(cy - ky);
            if (!(std::find(DIR_K.begin(), DIR_K.end(), vector<char>({dx, dy})) != DIR_K.end())) {
                dx = dx / abs(dx);
                dy = dy / abs(dx);
                char cx = kx, cy = ky;   // Current (x, y)
                while (true) {
                    cx += dx;
                    cy += dy;
                    const char loc = cy*8 + cx;
                    if (!bit(checking_pieces, loc)) block_mask = set_bit(block_mask, loc, true);
                    else break;
                }
            }

            // Go through all pieces and check if they can capture/block
            for (char i = 0; i < 64; i++) {
                if (bit(CP, i)) {
                    if (!pinned(CK, (1ULL << i), OP, OK, OB, OR, OQ, SAME)) {
                        // Capture
                        // todo en passant
                        const char x = i%8, y = i/8 + pos.turn ? 1 : -1;
                        if (0 <= y && y < 8) {
                            if (0 <= x-1 && x-1 < 8) {
                                const char char_move = y*8 + x-1;
                                const U64 bit_moves = 1ULL << char_move & capture_mask;
                                for (char j = 0; j < 64; j++) {
                                    if (bit(bit_moves, j)) moves.push_back(Move(i, j)); 
                                }
                            }
                            if (0 <= x+1 && x+1 < 8) {
                                const char char_move = y*8 + x+1;
                                const U64 bit_moves = 1ULL << char_move & capture_mask;
                                for (char j = 0; j < 64; j++) {
                                    if (bit(bit_moves, j)) moves.push_back(Move(i, j));
                                }
                            }
                        }
                        // Block
                        const char y = i/8;
                        const char speed = (y == (pos.turn ? 6 : 1)) ? 2 : 1;  // If white check rank 6 else rank 1 if on that rank 2 else 1
                        for (auto cy = y + 1; cy < y + speed + 1; cy++) {
                            const char loc = cy*8 + x;
                            if (bit(OPPOSITE | SAME, loc)) break;
                            if ((1ULL << loc) & block_mask) {
                                moves.push_back(Move(i, loc));
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            // todo Decide normal moves
        }
        return moves;
    }

    Position push(Position pos, Move move) {
        Position p;
        return p;
    }
}
