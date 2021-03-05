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
#include "board.hpp"
#include "constants.hpp"
#include "funcs.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


string piece_to_symbol(int piece) {
    switch (piece) {
        case WP: return "P";
        case WN: return "N";
        case WB: return "B";
        case WR: return "R";
        case WQ: return "Q";
        case WK: return "K";
        case BP: return "p";
        case BN: return "n";
        case BB: return "b";
        case BR: return "r";
        case BQ: return "q";
        case BK: return "k";
    }
    return " ";
}

int symbol_to_piece(string symbol) {
    switch (symbol[0]) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;
        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
    }
    return EM;
}

int letter_to_column(char letter) {
    return (letter >= 97) ? letter - 97 : letter - 65;
}

vector<int> string_to_square(string str) {
    int col = letter_to_column(str[0]);
    int row = 8 - std::stoi(str.substr(1, 1));
    return {row, col};
}

string square_to_string(vector<int> square, bool cap) {
    string column = cap ? string(1, square[1]+65) : string(1, square[1]+97);
    return column + std::to_string(8-square[0]);
}


int piece_value(int piece) {
    // not working with switch
    if      (piece == WP) return 1;
    else if (piece == WN) return 3;
    else if (piece == WB) return 3;
    else if (piece == WR) return 5;
    else if (piece == WQ) return 9;
    else if (piece == WK) return 0;
    else if (piece == BP) return 1;
    else if (piece == BN) return 3;
    else if (piece == BB) return 3;
    else if (piece == BR) return 5;
    else if (piece == BQ) return 9;
    else if (piece == BK) return 0;
}

bool piece_color(int piece) {
    return piece <= 6;
}

bool in_board(vector<int> loc) {
    return (0<=loc[0] && loc[0]<=7 && 0<=loc[1] && loc[1]<=7);
}


string strip(string str) {
    int start = str.find_first_not_of(" ");
    int end = str.find_last_not_of(" ");
    return str.substr(start, end-start+1);
}

string replace(string str, string rep) {
    int size = rep.size();
    while (true) {
        int pos = str.find(rep);
        if (pos == string::npos) break;
        str = str.substr(0, pos) + str.substr(pos+size);
    }
    return str;
}

vector<string> split(string str, string delim) {
    vector<string> parts;
    int prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string part = str.substr(prev, pos-prev);
        if (!part.empty()) parts.push_back(part);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return parts;
}

vector<int> addvecs(vector<int> v1, vector<int> v2) {
    if (v1.size() != v2.size()) cout << "Attempted to add two vectors of different sizes (file funcs.cpp)" << endl;
    vector<int> final;
    for (auto i = 0; i < v1.size(); i++) final.push_back(v1[i]+v2[i]);
    return final;
}
