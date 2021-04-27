![logo](https://raw.githubusercontent.com/megalodon-chess/megalodon/main/logo/logo_widescreen_light.png)

# Megalodon

![license](https://img.shields.io/github/license/megalodon-chess/megalodon)
![issues](https://img.shields.io/github/issues/megalodon-chess/megalodon)
![pulls](https://img.shields.io/github/issues-pr/megalodon-chess/megalodon)
![build](https://img.shields.io/github/workflow/status/megalodon-chess/megalodon/CI)
![release](https://img.shields.io/github/v/release/megalodon-chess/megalodon)
![stars](https://img.shields.io/github/stars/megalodon-chess/megalodon)
![forks](https://img.shields.io/github/forks/megalodon-chess/megalodon)
![size](https://img.shields.io/github/repo-size/megalodon-chess/megalodon)

UCI chess engine.

Play against Megalodon on [Lichess][lichess]!

[Version 1.0.0][latest] was recently released!

The source code of Megalodon is completely original. It is not a derivative or port of any other program.
We used articles from [chessprogramming.org](https://chessprogramming.org) and ideas from other engines.

## Compiling

### Compiling with automated Bash Script

Requires g++, CMake, and GNU Make.

``` bash
git clone https://github.com/megalodon-chess/megalodon.git
cd ./megalodon/
./build.sh
./build/Megalodon
```

### Compiling with CMake and GNU Make

Requires g++, CMake, and GNU Make.

``` bash
git clone https://github.com/megalodon-chess/megalodon.git
cd ./megalodon/
mkdir ./build/
cd ./build/
cmake ..
make -j
./Megalodon
```

### Compiling Manually

Requires g++.

``` bash
git clone https://github.com/megalodon-chess/megalodon.git
cd ./megalodon/src/
g++ -pthread -Ofast -Wall -Wextra *.cpp -o Megalodon
./Megalodon
```

## License

Everything in the repository is licensed as GNU GPL v3 license.

If you would like to use the logo, we request that you also include a message
saying that your project isn't related to Megalodon.

## The Name

The name Megalodon comes from a species of shark that lived millions of years ago.

## Documentation

For extra documentation, please read the [docs][docs].

[lichess]: https://lichess.org/@/megalodon-chess
[docs]: https://megalodon-chess.github.io/megalodon/
[latest]: https://github.com/megalodon-chess/megalodon/releases/latest
