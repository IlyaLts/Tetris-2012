/*
===============================================================================
    Copyright (C) 2012-2025 Ilya Lyakhovets

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

#include "Figure.h"

bool g_figures[NUMBER_OF_FIGURES][ROTATIONS_OF_FIGURE][FIGURE_HEIGHT][FIGURE_WIDTH] =
{
    // 1
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
    // 2
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
    // 3
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
    // 4
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
    // 5
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
    // 6
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
    // 7
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
Figure::Figure
===================
*/
Figure::Figure()
{
    numNext = libRandom::Int(0, NUMBER_OF_FIGURES - 1);
    New();
}

/*
===================
Figure::New
===================
*/
void Figure::New()
{
    pos.Set(0, 0);
    rot = 0;
    num = numNext;
    numNext = libRandom::Int(0, NUMBER_OF_FIGURES - 1);
    Update();
}

/*
===================
Figure::Rotate
===================
*/
void Figure::Rotate()
{
    rot++;

    if (rot == ROTATIONS_OF_FIGURE)
        rot = 0;

    Update();
}

/*
===================
Figure::Update
===================
*/
void Figure::Update()
{
    for (int i = 0; i < FIGURE_WIDTH; i++)
    {
        for (int j = 0; j < FIGURE_HEIGHT; j++)
        {
            blocks[i][j].filled = g_figures[num][rot][j][i];
            blocks[i][j].color = libCast<Block::Color>(num);
        }
    }
}
