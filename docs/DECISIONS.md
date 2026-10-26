# Project Decisions

This document records the architectural and gameplay decisions agreed upon by the team for the C++ Go Game project.

## 1. Technology Stack
* **Language & Standard:** C++17.
* **Build System:** CMake.
* **GUI Library:** Raylib (chosen for the frontend rendering and event loop).
* **Data Serialization:** `nlohmann/json` (header-only, for Save/Load functionality).

## 2. Board and Coordinates
* **Supported Board Sizes:** The supported board size is 19x19 initially. It can be extended to other sizes (e.g., 9x9) in the future.
* **Coordinate Convention:** Zero-based `(col, row)` indexing internally in the Game Engine (from `0` to `size - 1`), matching the current `Point` fields and `board[col][row]` representation. The Raylib UI will translate these abstract coordinates into screen pixels.

## 3. Game Rules and Mechanics
* **Starting Player:** Black always plays first.
* **Scoring System:** Chinese scoring (area scoring): count each living stone on the board and each empty intersection that is territory for that color. Captured stones are not added to the score. An empty region is not territory merely because it touches stones of only one color; it must be enclosed by living stones of that color. Board edges can form part of an enclosure but do not claim empty points by themselves. Regions that are open or bordered by both colors are neutral.
* **Komi:** 6.5 points for White to compensate for Black's first-move advantage.
* **Ko Rule:** Basic Ko rule is enforced (a player cannot immediately recreate the previous board state).
* **Suicide Policy:** Suicide moves (placing a stone that immediately has 0 liberties without capturing enemy stones) are strictly prohibited.

## 4. End-Game and Phase Management
* **Triggering the End-Game:** Two consecutive passes by the players will transition the game from the `"playing"` phase to the `"scoring"` phase.
* **Dead-Stone Selection:** The first version will not automatically determine life and death. Players should play out disputes before passing. If interactive dead-stone marking is added, marked stones must be removed for scoring and restored if scoring is canceled. The scoring implementation must not treat every empty region touching only one color as territory; unowned open regions remain neutral.
* **Undo Behavior (PvE):** In Player vs. AI mode, triggering "Undo" will reverse one human/AI *pair* of moves, returning the board directly to the human's turn. 

## 5. Persistence (Save/Load)
* **History Retention:** Loaded games will preserve the full Undo and Redo stacks.
* **Savable Phases:** The game can be saved during the `"playing"` and `"finished"` phases. 
* **Errors:** Failed save/load attempts (e.g., corrupted JSON, missing file) will display an error in the GUI and keep the current session intact without crashing.

## 6. AI Parameters
* **Response Budgets (Approximate):**
  * **Easy (Random):** 100 ms
  * **Medium (Minimax):** 300 ms
  * **Hard (Alpha-Beta):** 1,000 ms
* **Medium Search Bound:** Search to depth 2 (the AI move and the opponent's reply), considering at most 12 legal candidate moves at each ply. Rank legal moves by their immediate evaluation from the AI's perspective; keep the 12 highest-scoring moves on the AI's turn and the 12 lowest-scoring moves on the opponent's turn. Shuffle equal-scoring moves before sorting to vary tie choices. This bounds the search to at most 144 move sequences.
* **Cancellation:** A new action (like hitting Undo or resetting the board) while the AI is calculating will discard the stale AI output safely.
