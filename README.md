# Go Game

A desktop Go game project written in C++17 with a Raylib interface and a separate game-rules engine. It is being developed as a first-term final project for CS160 – Introduction to Computer Science.

## Team

- Tran Hung Tien
- Tran Viet Hung

## Current status

The project currently includes:

- A Raylib window, main menu, and board-size and game-mode selection screens.
- Board rendering for 9×9, 13×13, and 19×19 board selections.
- A game engine with stone placement, captures, liberties, suicide prevention, the basic ko rule, passing, scoring phases, territory proposals, and score acceptance.
- Chinese area scoring, with 6.5 komi for White.
- Session undo and redo history in the engine.
- JSON save/load functions for a game session, including its undo and redo history.
- Easy AI that chooses a random legal move, and Medium AI that uses a depth-2 minimax search with a limited candidate set.

The UI is still being connected to the engine. In particular, gameplay input, scoring review, save/load controls, undo/redo controls, and AI turns are not yet wired into the screens. Hard AI with alpha-beta pruning is planned but is not implemented yet.

## Requirements

- C++17 compiler
- CMake 3.14 or newer
- Git and an internet connection for the first CMake configure, which fetches Raylib 5.5 and nlohmann/json 3.12.0

## Build

From the repository root, configure and build with:

```sh
cmake -S . -B build
cmake --build build --config Debug
```

CMake copies the `assets` directory beside the built executable. With the Visual Studio generator on Windows, run:

```powershell
.\build\Debug\GoGame.exe
```

With a single-configuration generator such as Ninja or Make, the executable is typically at `build/GoGame` (or `build/GoGame.exe` on Windows).

## Source layout

- `include/`, `src/` – application, UI, board renderer, game engine, AI, and persistence code.
- `assets/` – images, fonts, and other interface resources.
- `docs/DECISIONS.md` – agreed project and gameplay decisions.

## Project decisions

Black moves first. The internal coordinate convention is zero-based `(col, row)`, matching `board[col][row]`. The initial project target is a 19×19 Go game, while the current UI also offers 9×9 and 13×13 board selections. Rules and design choices are recorded in [`docs/DECISIONS.md`](docs/DECISIONS.md).
