# Go Game (C++)

A desktop Go game application built in C++ supporting both Player vs. Player and Player vs. AI modes.
Developed as the first-term final project for CS160 - Introduction to Computer Science.

## Team Members
* Tran Hung Tien
* Tran Viet Hung

## Features (In Progress)
* 19x19 board support
* Chinese scoring rules
* Player vs Player mode
* Save/Load game state using JSON
* AI Opponent (Easy, Medium, Hard) using Minimax & Alpha-Beta pruning

## Dependencies
* **C++17** or higher
* **CMake** (3.10+)
* **Raylib** (for UI/graphics)

## Build Instructions
To build and run this project from a fresh clone, use the following commands in your terminal:

```bash
# 1. Clone the repository
git clone [https://github.com/trvhung058/GoGame.git](https://github.com/trvhung058/GoGame.git)
cd GoGame

# 2. Generate the build files
cmake -S . -B build

# 3. Compile the project
cmake --build build --config Debug

# 4. Run the executable (Linux/macOS)
./build/GoGame
# Or on Windows:
.\build\Debug\GoGame.exe