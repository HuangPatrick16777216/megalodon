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
#include "uci.hpp"
#include "bitboard.hpp"
#include "random.hpp"
#include "hash.hpp"
#include "eval.hpp"
#include "search.hpp"
#include "utils.hpp"

#define VERSION  "1.0.0"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


void print_info() {
    cout << "Megalodon v" << VERSION << " - UCI chess engine" << "\n";
    cout << "Build date: " << __DATE__ << ", " << __TIME__ << endl;
    cout << "Copyright the Megalodon developers (in AUTHORS file)" << "\n";
    cout << "https://github.com/megalodon-chess/megalodon" << "\n";
    cout << "Licensed under GNU GPL v3: https://www.gnu.org/licenses/" << "\n";
}


void bench() {
    const Options options;
    constexpr char num_pos = 20;
    constexpr char depth = 4;
    const string fens[num_pos] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r2q1rk1/ppp2pp1/2np1n1p/2b1p3/2B1P1b1/2NPBN2/PPPQ1PPP/R3R1K1 b Qq - 0 1",
        "6k1/5ppp/8/8/8/8/5PPP/6K1 w - - 0 1",
        "8/8/NnNnNnNn/8/8/8/8/2k2K2 b - - 0 1",
        "7k/p6P/p6P/p6P/p6P/p6P/p6P/K7 w - - 0 1",
        "8/8/8/8/8/2k5/8/1QK5 w - - 0 1",
        "4k3/p7/Pp6/1Pp1p3/2PpPp1p/3P1PpP/6P1/3K4 w - - 0 1",
        "8/1k6/3K4/2P5/8/8/8/8 w - - 0 1",
        "r2q1rk1/pbp1bpp1/1pn1pn1p/3p4/2PP3B/1PN1PN2/P3BPPP/R2Q1RK1 b Qq - 0 1",
        "2k4r/1pp5/p7/2p1N3/3n1P2/7P/6P1/R5K1 b - - 0 1",
        "7k/p7/8/8/8/8/P7/7K b - - 0 1",
        "8/8/8/3b4/8/3Bk3/1BK2b2/8 b - - 0 1",
        "8/3qPK2/8/8/8/4k3/8/8 w - - 0 1",
        "8/7P/5K2/8/8/4k3/p7/8 w - - 0 1",
        "r5k1/2p2ppp/pp2n3/8/P7/1B1P3P/1PP2PP1/3R1RK1 b - - 0 1",
        "4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 b - - 0 1",
        "r1r1r1r1/k7/8/8/8/8/1K6/1R1R1R1R w - - 0 1",
        "K7/8/8/3P4/4p3/8/8/7k b - - 0 1",
        "8/kpP3p1/p2n2P1/3Q3P/5q2/P7/KP6/8 w - - 0 1",
        "8/8/8/8/8/k7/8/K7 w - - 0 1",
    };

    U64 nodes = 0;
    const double start = get_time();
    bool searching = true;

    for (UCH i = 0; i < num_pos; i++) {
        cout << "Position " << i+1 << " of " << +num_pos << endl;
        cout << "Fen: " << fens[i] << endl;

        const Position pos = Bitboard::parse_fen(fens[i]);
        const SearchInfo result = Search::search(options, pos, depth, 10000, false, searching);
        nodes += result.nodes;
    }

    const double elapse = get_time() - start;
    const int nps = nodes / elapse;

    cout << "\nBenchmark results:" << endl;
    cout << "Nodes: " << nodes << endl;
    cout << "NPS: " << nps << endl;
    cout << "Time: " << elapse << " seconds" << endl;
}


int main(const int argc, const char* argv[]) {
    cout << std::fixed;
    Random::set_seed(1234);

    if (argc >= 2) {
        if      (argv[1] == string("--version")) cout << VERSION << endl;
        else if (argv[1] == string("bench")) bench();
    } else {
        Hash::init();
        Eval::init();

        print_info();
        return loop();
    }
}
