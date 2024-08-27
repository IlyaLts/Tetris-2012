/*
===============================================================================
    Copyright (C) 2012 Ilya Lyakhovets

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/

#pragma once

#include "Main.h"

constexpr int tetrominoAmount = 7;
constexpr int rotations = 4;
constexpr int tetrominoWidth = 4;
constexpr int tetrominoHeight = 4;

extern bool g_tetrominos[tetrominoAmount][rotations][tetrominoWidth][tetrominoHeight];

struct Block
{
    enum Color
    {
        Cyan,
        Blue,
        Orange,
        Yellow,
        Green,
        Purple,
        Red,
    };

    Color color;
    bool filled;
};

/*
===============================================================================

    Tetromino

===============================================================================
*/
class Tetromino
{
public:
            
                    Tetromino();

    void            New();
    void            Rotate();
    void            Update();
    
    const auto &    Blocks() const { return blocks; }

    const auto &    NextRotation() const { return g_tetrominos[id][(rot + 1) % rotations]; }
    const auto &    NextTetromino() const { return g_tetrominos[nextId][0]; }
    int             NextID() const { return nextId; }

    libVec2i        pos;

private:

    Block           blocks[tetrominoWidth][tetrominoHeight];
    int             rot;
    int             id;
    int             nextId;
};
