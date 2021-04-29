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
#include "options.hpp"
#define HASH_FACTOR  348000


Transposition::Transposition() {
}

Options::Options() {
    Hash           = 256;
    EvalMaterial   = 1;
    EvalPawnStruct = 1;
    EvalSpace      = 1;
    EvalKnights    = 1;
    EvalRooks      = 1;
    EvalQueens     = 1;
    EvalKings      = 1;
    hash_table = new Transposition[16];
    set_hash();
}
void Options::set_hash() {
    delete[] hash_table;
    hash_size = Hash * HASH_FACTOR;
    hash_table = new Transposition[hash_size];
    clear_hash();
}
void Options::clear_hash() {
    for (unsigned int i = 0; i < hash_size; i++) {
        hash_table[i].depth = 0;
    }
}
