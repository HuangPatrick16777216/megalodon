# Compiling Megalodon

To compile Megalodon from it's source code, follow these steps:

## Requirements

* GNU Make
* CMake
* g++

## Compiling With Automated Bash Script

Requires g++, CMake, and GNU Make.

``` bash
git clone https://github.com/megalodon-chess/megalodon.git
cd ./megalodon/
./build.sh
./build/Megalodon
```

## Compiling With CMake and GNU Make

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

## Compiling Manually

Requires g++.

``` bash
git clone https://github.com/megalodon-chess/megalodon.git
cd ./megalodon/src/
g++ -pthread -Ofast -Wall -Wextra *.cpp -o Megalodon
./Megalodon
```

[Back to documentation home][home]

[home]: https://megalodon-chess.github.io/megalodon/
