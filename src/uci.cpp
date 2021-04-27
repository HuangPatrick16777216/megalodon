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
#include <thread>
#include "utils.hpp"
#include "bitboard.hpp"
#include "search.hpp"
#include "options.hpp"
#include "eval.hpp"
#include "perft.hpp"
#include "hash.hpp"
#include "endgame.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::to_string;

const vector<string> GREETINGS = {"Hello!", "Lets play!", "Are you ready for a game?"};
const vector<string> WINNING = {"Looks like I'm playing well!", "Wow!", "This is great!"};
const vector<string> LOSING = {"Oh no!", "I blundered.", "Nice play!", "Great job!", "*sigh*. You're good."};
const vector<string> GAME_END = {"Good game!", "I look forward to playing again.", "Want to play another one?", "Rematch?"};


Position parse_pos(const string& str) {
    const vector<string> parts = split(str, " ");
    if (parts[1] == "startpos") {
        Position pos = Bitboard::startpos();
        if (parts.size() > 3 && parts[2] == "moves") {
            for (UCH i = 3; i < parts.size(); i++) {
                pos = Bitboard::push(pos, parts[i]);
            }
        }
        return pos;
    } else if (parts[1] == "fen") {
        string fen;
        for (char i = 2; i < 8; i++) {
            fen += parts[i];
            fen += " ";
        }
        Position pos = Bitboard::parse_fen(fen);

        if (parts.size() > 9 && parts[8] == "moves") {
            for (UCH i = 9; i < parts.size(); i++) {
                pos = Bitboard::push(pos, parts[i]);
            }
        }
        return pos;
    }
    return Position();
}


void print_legal_moves(const Position& pos) {
    const vector<Move> moves = Bitboard::legal_moves(pos, Bitboard::attacked(pos, !pos.turn));
    cout << moves.size() << endl;
    for (const auto& m: moves) cout << Bitboard::move_str(m) << "\n";
}


float go(const Options& options, const Position& pos, const vector<string>& parts, bool& searching) {
    int mode = 0;
    int depth = 99;
    double movetime;
    float wtime = 0, btime = 0, winc = 0, binc = 0;
    bool infinite = false;
    for (UCH i = 0; i < parts.size()-1; i++) {
        if (parts[i] == "depth") {
            mode = 1;
            depth = std::stoi(parts[i+1]);
        } else if (parts[i] == "wtime") {
            mode = 2;
            wtime = std::stof(parts[i+1]) / 1000;
        } else if (parts[i] == "btime") {
            mode = 2;
            btime = std::stof(parts[i+1]) / 1000;
        } else if (parts[i] == "winc") {
            mode = 2;
            winc = std::stof(parts[i+1]) / 1000;
        } else if (parts[i] == "binc") {
            mode = 2;
            binc = std::stof(parts[i+1]) / 1000;
        } else if (parts[i] == "movetime") {
            mode = 3;
            movetime = std::stof(parts[i+1]) / 1000;
        } else if (parts[i] == "infinite") {
            infinite = true;
        }
    }

    if (mode == 0) {
        depth = 99;
        movetime = 10000000;  // About 100 days
    } else if (mode == 1) {
        movetime = 10000000;
    } else if (mode == 2) {
        depth = 99;
        if (pos.turn) movetime = Search::move_time(pos, wtime, winc);
        else          movetime = Search::move_time(pos, btime, binc);
    }
    if (mode == 2) movetime /= 1.5;

    searching = true;
    const SearchInfo result = Search::search(options, pos, depth, movetime, infinite, searching);
    cout << "bestmove " << Bitboard::move_str(result.pv.front()) << endl;

    return result.score;
}

void perft(const Position& pos, const int& depth) {
    const vector<Move> moves = Bitboard::legal_moves(pos, Bitboard::attacked(pos, !pos.turn));
    const double start = get_time();
    long long nodes = 0;

    if (moves.size() > 0) {
        int move_num = 1;
        for (const auto& move: moves) {
            const Position new_pos = Bitboard::push(pos, move);
            const long long curr_nodes = Perft::movegen(new_pos, depth-1);
            nodes += curr_nodes;
            cout << "info currmove " << Bitboard::move_str(move) << " currmovenumber " << move_num << " nodes " << curr_nodes << endl;
            move_num++;
        }
    }

    const double elapse = get_time() - start + 0.001;  // Add 1 ms to prevent divide by 0
    cout << "info depth " << depth << " nodes " << nodes << " nps " << (long long)(nodes/elapse) << " time " << (long long)(elapse*1000) << endl;
}

void perft_hash(const Position& pos, const int& knodes) {
    const double time = Perft::hash_perft(pos, knodes);
    cout << "info nodes " << 1000*knodes << " nps " << (int)(knodes*1000/time) << " time " << (int)(time*1000) << endl;
}

void perft_eval(const Options& options, const Position& pos, const int& knodes) {
    const double time = Perft::eval_perft(options, pos, knodes);
    cout << "info nodes " << 1000*knodes << " nps " << (int)(knodes*1000/time) << " time " << (int)(time*1000) << endl;
}

void perft_push(const Position& pos, const int& knodes) {
    const double time = Perft::push_perft(pos, knodes);
    cout << "info nodes " << 1000*knodes << " nps " << (int)(knodes*1000/time) << " time " << (int)(time*1000) << endl;
}


int loop() {
    string cmd;
    Options options;
    Position pos = parse_pos("position startpos");
    bool searching = false;

    while (getline(cin, cmd)) {
        cmd = strip(cmd);

        if (cmd == "quit") break;
        else if (cmd == "clear") cout << "\x1b[3J\x1b[H\x1b[2J" << std::flush;
        else if (cmd == "isready") cout << "readyok" << endl;
        else if (cmd == "uci") {
            cout << "id name Megalodon\n";
            cout << "id author Megalodon Developers\n";

            cout << "option name Hash type spin default 256 min 1 max 65536\n";

            cout << "option name EvalMaterial type spin default 100 min 0 max 1000\n";
            cout << "option name EvalPawnStruct type spin default 100 min 0 max 1000\n";
            cout << "option name EvalSpace type spin default 100 min 0 max 1000\n";
            cout << "option name EvalKnights type spin default 100 min 0 max 1000\n";
            cout << "option name EvalRooks type spin default 100 min 0 max 1000\n";
            cout << "option name EvalQueens type spin default 100 min 0 max 1000\n";
            cout << "option name EvalKings type spin default 100 min 0 max 1000\n";

            cout << "uciok" << endl;
        }
        else if (startswith(cmd, "setoption")) {
            const vector<string> parts = split(cmd, " ");
            const string name = parts[2];
            const string value = parts[4];

            if (name == "Hash") {
                options.Hash = std::stoi(value);
                options.set_hash();
            }

            else if (name == "EvalMaterial")   options.EvalMaterial   = std::stof(value)/100;
            else if (name == "EvalPawnStruct") options.EvalPawnStruct = std::stof(value)/100;
            else if (name == "EvalSpace")      options.EvalSpace      = std::stof(value)/100;
            else if (name == "EvalKnights")    options.EvalKnights    = std::stof(value)/100;
            else if (name == "EvalRooks")      options.EvalRooks      = std::stof(value)/100;
            else if (name == "EvalQueens")     options.EvalQueens     = std::stof(value)/100;
            else if (name == "EvalKings")      options.EvalKings      = std::stof(value)/100;

            else std::cerr << "Unknown option: " << name << endl;
        }

        else if (cmd == "d") cout << Bitboard::board_str(pos) << endl;
        else if (startswith(cmd, "hash")) {
            const vector<string> parts = split(cmd, " ");
            if (parts.size() == 1) {
                cout << Hash::hash(pos) << endl;
            } else if (parts[1] == "perft" && parts.size() >= 2) {
                perft_hash(pos, std::stoi(parts[2]));
            }
        }
        else if (startswith(cmd, "eval")) {
            const vector<string> parts = split(cmd, " ");
            if (parts.size() == 1) {
                const U64 attacked = Bitboard::attacked(pos, !pos.turn);
                Eval::eval(options, pos, Bitboard::legal_moves(pos, attacked), 0, attacked, true);
            } else if (parts[1] == "perft" && parts.size() >= 2) {
                perft_eval(options, pos, std::stoi(parts[2]));
            }
        }
        else if (cmd == "legalmoves") print_legal_moves(pos);
        else if (startswith(cmd, "pushperft")) {
            const vector<string> parts = split(cmd, " ");
            perft_push(pos, std::stoi(parts[1]));
        }
        else if (cmd == "eg") cout << Endgame::eg_type(pos) << endl;

        else if (cmd == "ucinewgame") pos = parse_pos("position startpos");
        else if (startswith(cmd, "position")) pos = parse_pos(cmd);
        else if (startswith(cmd, "go")) {
            const vector<string> parts = split(cmd, " ");
            if (parts.size() > 1 && parts[1] == "perft") perft(pos, std::stoi(parts[2]));
            else {
                options.clear_hash();
                searching = true;
                std::thread(go, options, pos, parts, std::ref(searching)).detach();
            }
        }
        else if (cmd == "stop") searching = false;
        else if (cmd.size() > 0) std::cerr << "Unknown command: " << cmd << endl;
    }

    searching = false;
    delete[] options.hash_table;
    return 0;
}
