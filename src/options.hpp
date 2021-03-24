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

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

class Options {
/*
Hash: type=spin, default=16, min=1, max=65536, hash table size (megabytes)
EvalMaterial: type=spin, default=100, min=0, max=1000, weight (percent) of material eval.
Chat: type=check, default=true, whether the engine should chat with you.
*/

public:
    Options();
    void set_hash();

    bool* hash_evaled = new bool[16];
    float* hash_evals = new float[16];
    int hash_size;

    int Hash;
    int EvalMaterial;
    bool Chat;
};
