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

#include "Tetromino.h"

bool g_tetrominos[tetrominoAmount][rotations][tetrominoHeight][tetrominoWidth] =
{
    // Straight
    {
        {
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
        },
        {
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
        },
    },
    // J
    {
        {
            1, 1, 1, 0,
            0, 0, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 0, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            0, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
    },
    // L
    {
        {
            1, 1, 1, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 0, 1, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
    },
    // Square
    {
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
    },
    // S-skew
    {
        {
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
    },
    // T
    {
        {
            1, 1, 1, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            0, 1, 1, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
        },
    },
    // Z-skew
    {
        {
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
        },
        {
            0, 1, 0, 0,
            1, 1, 0, 0,
            1, 0, 0, 0,
            0, 0, 0, 0,
        }
    }
};

/*
===================
Tetromino::Tetromino
===================
*/
Tetromino::Tetromino()
{
    nextId = libRandom::Int(0, tetrominoAmount - 1);
    New();
}

/*
===================
Tetromino::New
===================
*/
void Tetromino::New()
{
    pos.Set(0, 0);
    rot = 0;
    id = nextId;
    nextId = libRandom::Int(0, tetrominoAmount - 1);
    Update();
}

/*
===================
Tetromino::Rotate
===================
*/
void Tetromino::Rotate()
{
    rot = (rot + 1) % rotations;
    Update();
}

/*
===================
Tetromino::Update
===================
*/
void Tetromino::Update()
{
    for (int i = 0; i < tetrominoWidth; i++)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            blocks[i][j].filled = g_tetrominos[id][rot][j][i];
            blocks[i][j].color = libCast<Block::Color>(id);
        }
    }
}
