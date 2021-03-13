# Releases

This page contains release info.

## [Megalodon 0.1.0][v0.1.0]

A few hours of working after v0.1.0-alpha, v0.1.0 is out!

The move generation algorithm is almost fully working, and sometimes misses en passant moves.

It can generate moves at 550,000 NPS (without minimax or evaluation),
and the final playing NPS is about 35,000.

The engine is set to run DFS search at depth 4 each move. It thinks for 5-15 seconds.

The next releases (v0.1.x) will be focused on developing the search algorithm.

## [Megalodon 0.1.0 Alpha][v0.1.0-alpha]

This is a pre-release to Megalodon 0.1.0, which will feature fully working bitboards!

This version does not function fully, and will likely generate illegal moves.

## [Megalodon 0.0.3][v0.0.3]

This release features improved board algorithms, which increase it's NPS (nodes per second) from 4000 to 8000.

The depth of calculation was also adjusted to 4, and Megalodon will think for about 30 seconds per move.

After this release, the Megalodon team will work on implementing bitboards.

## [Megalodon 0.0.2][v0.0.2]

This release includes updated searching algorithms and bug fixes.

## [Megalodon 0.0.1][v0.0.1]

This is the first release of Megalodon, a UCI chess engine! It does not function fully yet.

Megalodon uses a minimax search with material and piece maps as evaluation.

[Back to documentation home][home]

[home]: https://huangpatrick16777216.github.io/megalodon/
[v0.1.0]: https://github.com/HuangPatrick16777216/megalodon/releases/tag/v0.1.0
[v0.1.0-alpha]: https://github.com/HuangPatrick16777216/megalodon/releases/tag/v0.1.0-alpha
[v0.0.3]: https://github.com/HuangPatrick16777216/megalodon/releases/tag/v0.0.3
[v0.0.2]: https://github.com/HuangPatrick16777216/megalodon/releases/tag/v0.0.2
[v0.0.1]: https://github.com/HuangPatrick16777216/megalodon/releases/tag/v0.0.1
