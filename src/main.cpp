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
#include "constants.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;


void loop() {
    string cmd;

    while (getline(std::cin, cmd)) {
        if (cmd == "quit") break;
        else if (cmd == "isready") std::cout << "readyok" << std::endl;
        else if (cmd == "uci") std::cout << "uciok" << std::endl;
        else if (cmd == "d");

        else if (cmd.substr(0, 2) == "go") std::cout << "bestmove e2e4" << std::endl;
    }
}


int main() {
    loop();
}
