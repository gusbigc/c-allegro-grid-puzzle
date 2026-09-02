# C Grid Puzzle Game (Allegro 5)

A grid-based color-matching puzzle game developed in **C** using the **Allegro 5** library, featuring row/column rotation, gravity mechanics, and a phase/score progression system.

---

## About the game

> **context:** This project was developed during my **1st semester** of university for the *Programming Laboratory* course. It is preserved in this repository as a historical record of my learning journey in software development and structured programming in C.

---

## Features & Gameplay Mechanics

* **Color Grid (5x6):** Dynamic board with randomly generated colors.
* **Rotation System:** Allows shifting rows (horizontally) and columns (vertically/crosswise).
* **Gravity Mechanics:** Pieces fall to fill empty spaces after clears.
* **Level Progression:** 5 levels with an increasing number of colors and a time limit.
* **High Score System:** Saves scores to a local file (`recordes.txt`).

---

## Controls

> make sure`CAPS LOCK` is turned off while playing.

| Key | Action |
| :--- | :--- |
| `Tab` | Toggle selection between **Row Mode** and **Column Mode** |
| `W` / `S` | Move selection up/down (Row Mode) |
| `A` / `D` | Move selection left/right (Column Mode) |
| `Q` / `E` | Rotate selected row or column |
| `B` / `C` | Shift column down/up |
| `Space` | Clear selected row *(incurs point penalty)* |
| `ESC` | Exit game |

---

## 🛠️ Tech Stack

* **Language:** C (C99)
* **Graphics Library:** Allegro 5 (Primitives, Fonts, TTF, Image)

---

## 📦 How to Compile and Run

### Prerequisites
Ensure you have the `gcc` compiler installed along with the **Allegro 5** libraries.

### Compilation
Run the following command in your terminal inside the project folder:

```bash
gcc game.c janela.c animacao.c -o grid_game -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image
