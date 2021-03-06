/*
===============================================================================
    Copyright (C) 2012-2015 Ilya Lyakhovets

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

#ifndef __GLASS_H__
#define __GLASS_H__

#include "Figure.h"

#define NUMBER_OF_LEVELS			15
#define FIGURE_FALL_DELAY			1.0f
#define FIGURE_FALL_DELAY_DECREASE	0.05f

#define SCORE_FOR_1_LINE			10
#define SCORE_FOR_2_LINES			30
#define SCORE_FOR_3_LINES			60
#define SCORE_FOR_4_LINES			100
#define	STARTUP_SCORE_GOAL			100
#define SCORE_GOAL_MULTIPLIER		2
#define MAX_SCORE					999999

#define BLOCK_WIDTH					32
#define BLOCK_HEIGHT				32
#define BLOCK_GHOST_COLOR			libClr(1.0f, 1.0f, 1.0f, 0.33f)

#define GLASS_WIDTH					10
#define GLASS_HEIGHT				20

/*
===============================================================================

	Glass

===============================================================================
*/
class Glass
{
public:

				Glass();

	bool		Init();
	void		Free();

	void		Draw(float x, float y);
	void		Update();

private:

	void		SaveRecord();
	
	void		DrawBlock(float x, float y, color_t clrType, const libClr &clr = libClr()) const;
	void		DrawRectangleOutline(const libQuad &quad, const libClr &clr) const;

	void		NewGame();
	void		BuildFigureIntoGlass();
	void		RemoveFilledLines();
	bool		IsFigureDropped() const;
	bool		IsThereFreeSpaceForNewFigure() const;

	void		MoveLeft();
	void		MoveRight();
	void		MoveDown();
	void		DropFigure();
	void		RotateFigure();

	libFont *	font;
	libTexture *blocks;
	libSound *	drop;

	Block		glass[GLASS_WIDTH][GLASS_HEIGHT];
	Figure		figure;

	bool		gameOver;
	bool		help;
	bool		ghostEnabled;
	libHolder	score;
	libHolder	scoreGoal;
	libHolder	level;
	libHolder	record;

	float		keyDelay;
	libTimer	fallDelay;
	libTimer	leftFigureDelay;
	libTimer	rightFigureDelay;
	libTimer	downFigureDelay;

	libCfg		cfg;
};

#endif // !__GLASS_H__
