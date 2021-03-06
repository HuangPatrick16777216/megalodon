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

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include "board.hpp"
#include "constants.hpp"
#include "funcs.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


Board::Board() {
    reset();
}

Board::Board(string fen) {
    set_fen(fen);
    _move_stack = {};
}

void Board::reset() {
    _board = {
        {BR, BN, BB, BQ, BK, BB, BN, BR},
        {BP, BP, BP, BP, BP, BP, BP, BP},
        {EM, EM, EM, EM, EM, EM, EM, EM},
        {EM, EM, EM, EM, EM, EM, EM, EM},
        {EM, EM, EM, EM, EM, EM, EM, EM},
        {EM, EM, EM, EM, EM, EM, EM, EM},
        {WP, WP, WP, WP, WP, WP, WP, WP},
        {WR, WN, WB, WQ, WK, WB, WN, WR}
    };
    _turn = true;
    _castling = {true, true, true, true};
    _ep = false;
    _move_stack = {};
}

string Board::as_string() {
    string str;
    str += " " + BOARD_OUTROW + "\n";
    for (auto i = 0; i < 8; i++) {
        str += BOARD_OUTCOL;
        for (auto piece: _board[i]) {
            string symbol = piece_to_symbol(piece);
            str += symbol + BOARD_OUTCOL;
        }
        str += std::to_string(8-i) + "\n";
        str += " " + BOARD_OUTROW + "\n";
    }
    str += "   ";
    for (auto i: "abcdefgh") str += string(1, i) + "   ";

    str += "\n\nFen: " + fen();
    return str;
}

vector<vector<int>> Board::board() {
    return _board;
}

vector<Move> Board::move_stack() {
    return _move_stack;
}

void Board::set_fen(string fen) {
    vector<string> parts = split(fen, " ");
    string position = parts[0];
    string turn = parts[1];
    string castling = parts[2];
    string ep = parts[3];

    _board = {};
    vector<int> row = {};
    for (auto i = 0; i < position.size(); i++) {
        char ch = position[i];
        if (ch == '/') {
            _board.push_back(row);
            row = {};
        } else {
            if (48 <= ch && ch <= 57) {
                for (auto j = 0; j < ch-48; j++) row.push_back(EM);
            } else {
                row.push_back(symbol_to_piece(string(1, ch)));
            }
        }
    }
    _board.push_back(row);

    _turn = (turn == "w");

    for (auto i: castling) {
        int ind;
        switch (i) {
            case 'K': ind = 0; break;
            case 'Q': ind = 1; break;
            case 'k': ind = 2; break;
            case 'q': ind = 3; break;
        }
        _castling[ind] = true;
    }

    _ep = (ep != "-");
    if (_ep) _ep_square = string_to_square(ep);
}

string Board::fen() {
    string fen;
    int space = 0;

    for (auto row = 0; row < 8; row++) {
        for (auto col = 0; col < 8; col++) {
            int sq = _board[row][col];
            if (sq != EM) {
                if (space) fen += std::to_string(space);
                fen += piece_to_symbol(sq);
                space = 0;
            } else {
                space++;
            }

        }
        if (space) fen += std::to_string(space);
        space = 0;
        fen += "/";
    }
    fen.pop_back();

    fen += " ";
    fen += _turn ? "w" : "b";

    bool contains = false;
    for (auto c: _castling) {
        if (c) {
            contains = true;
            break;
        }
    }
    fen += " ";
    if (contains) {
        for (auto i = 0; i < 4; i++) {
            if (_castling[i]) fen += CASTLING_SYMBOLS[i];
        }
    } else {
        fen += "-";
    }

    fen += " ";
    fen += _ep ? square_to_string(_ep_square) : "-";

    fen += " 0 1";
    return fen;
}

void Board::push(Move move) {
    vector<int> to = move.to_square();
    vector<int> from = move.from_square();

    _board[to[0]][to[1]] = move.promotion() ? move.promo_piece() : _board[from[0]][from[1]];
    _board[from[0]][from[1]] = EM;
    _turn = !_turn;
    _move_stack.push_back(move);
}

void Board::push_uci(string str) {
    push(Move(str));
}

vector<int> Board::king_pos(bool side) {
    int k = side ? WK : BK;
    for (auto row = 0; row < 8; row++) {
        for (auto col = 0; col < 8; col++) {
            if (_board[row][col] == k) return {row, col};
        }
    }
}

vector<Move> Board::_calc_sliding_moves(vector<int> sq, vector<vector<int>> dirs, const int max_dist = 8) {
    vector<Move> moves;
    const string from = square_to_string(sq);

    for (auto dir: dirs) {
        // Loop till edge of board
        for (auto i = 1; i < max_dist; i++) {
            // Move in the direction for i distance
            vector<int> sum = {0, 0};
            for (auto j = 0; j < i; j++) sum = addvecs(sum, dir);
            vector<int> new_sq = addvecs(sq, sum);

            if (!in_board(new_sq)) break;  // break if dir has reached out of board
            int piece = _board[new_sq[0]][new_sq[1]];
            if (piece != EM) {
                if (_turn == piece_color(piece)) break;
                moves.push_back(Move(from + square_to_string(new_sq)));
                break;
            } else {
                moves.push_back(Move(from + square_to_string(new_sq)));
            }
        }
    }
    return moves;
}

vector<Move> Board::_calc_jump_moves(vector<int> sq, vector<vector<int>> jumps) {
    vector<Move> moves;
    const string from = square_to_string(sq);

    // For each endpoint in jumps
    for (auto end: jumps) {
        // Check that position
        vector<int> pos = addvecs(sq, end);
        // If position is inside board
        if (in_board(pos)) {
            int piece = _board[pos[0]][pos[1]];
            // If the piece is empty, or of opposite color; add move
            if (piece == EM || _turn != piece_color(piece)) moves.push_back(Move(from + square_to_string(pos)));
        }
    }
    return moves;
}

vector<Move> Board::rook_moves(vector<int> sq) {
    return _calc_sliding_moves(sq, {{0, 1}, {-1, 0}, {1, 0}, {0, -1}});
}

vector<Move> Board::bishop_moves(vector<int> sq) {
    return _calc_sliding_moves(sq, {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}});
}

vector<Move> Board::queen_moves(vector<int> sq) {
    return _calc_sliding_moves(sq, {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {0, 1}, {-1, 0}, {1, 0}, {0, -1}});
}

vector<Move> Board::knight_moves(vector<int> sq) {
    return _calc_jump_moves(sq, {{-2, 1}, {2, 1}, {-2, -1}, {2, -1}, {1, -2}, {-1, 2}, {-1, -2}, {1, 2}});
}

vector<Move> Board::king_moves(vector<int> sq) {
    return _calc_jump_moves(sq, {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}});
}

vector<Move> Board::pawn_moves(vector<int> sq) {
    vector<Move> moves;

    if (_turn) moves = _calc_sliding_moves(sq, {{-1, 0}}, sq[1] == 6 ? 2 : 1);  // If pawn is on first rank then move 2 else 1
    else moves = _calc_sliding_moves(sq, {{1, 0}}, sq[1] == 1 ? 2 : 1);  // If pawn is on seventh rank then move 2 else 1
}
