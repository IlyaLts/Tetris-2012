/*
===============================================================================
    Copyright (C) 2012-2022 Ilya Lyakhovets

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

#ifndef __GAME_H__
#define __GAME_H__

#include "Figure.h"

#define FIELD_WIDTH					10
#define FIELD_HEIGHT				20
#define BLOCK_SIZE					32
#define HELPER_COLOR				libColor(1.0f, 1.0f, 1.0f, 0.33f)

#define FIGURE_FALL_START_DELAY		1.0f
#define FIGURE_FALL_DECREASE_COEFF	0.4f
#define DEFAULT_KEY_DELAY			0.18f
#define DEFAULT_SOUND_ENABLED		true
#define DEFAULT_HELPER_ENABLED		false

#define SCORE_FOR_1_LINE			10
#define SCORE_FOR_2_LINES			25
#define SCORE_FOR_3_LINES			50
#define SCORE_FOR_4_LINES			100
#define	STARTUP_SCORE_GOAL			100
#define SCORE_GOAL_MULTIPLIER		2
#define MAX_SCORE					99999999

/*
===============================================================================

	Game

===============================================================================
*/
class Game
{
public:

				Game();

	bool		Init();
	void		Draw();
	void		Update();
	void		Free();

private:

	void		SaveRecord();
	
	void		DrawBlock(float x, float y, Block::Color colorType, const libColor &color = libColor()) const;
	void		DrawBorder(const libQuad &quad, const libColor &color) const;

	void		NewGame();
	void		RemoveFilledLines();
	bool		IsFigureDropped() const;
	bool		IsThereSpaceForNewFigure() const;

	void		MoveLeft();
	void		MoveRight();
	void		MoveDown();
	void		DropFigure();
	void		RotateFigure();

	libFont *	font;
	libTexture *blocks;
	libSound *	drop;

	Block		field[FIELD_WIDTH][FIELD_HEIGHT];
	Figure		figure;

	bool		gameOver;
	bool		help;
	bool		helperEnabled;
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

#endif // !__GAME_H__
