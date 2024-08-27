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

#include "Main.h"

#ifndef __FIGURE_H__
#define __FIGURE_H__

#define NUMBER_OF_FIGURES       7
#define ROTATIONS_OF_FIGURE     4
#define FIGURE_WIDTH            4
#define FIGURE_HEIGHT           4

extern bool g_figures[NUMBER_OF_FIGURES][ROTATIONS_OF_FIGURE][FIGURE_WIDTH][FIGURE_HEIGHT];

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

    Figure

===============================================================================
*/
class Figure
{
public:
            
                Figure();

    void        New();
    void        Rotate();
    void        Update();

    Block       blocks[FIGURE_WIDTH][FIGURE_HEIGHT];
    libVec2i    pos;
    int         rot;
    int         num;
    int         numNext;
};

#endif // !__FIGURE_H__
