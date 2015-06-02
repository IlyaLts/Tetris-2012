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

#include "Main.h"
#include "Glass.h"

/*
===================
Glass::Glass
===================
*/
Glass::Glass()
{
	help = false;
	font = nullptr;
	drop = nullptr;
}

/*
===================
Glass::Init
===================
*/
bool Glass::Init()
{
	if (!engine->Get_libFont(font, DATA_PACK "Font.png", true)) return false;
	if (!engine->Get_libSound(drop, DATA_PACK "Drop.wav", false, true)) return false;
	if (!engine->Get_libTexture(blocks, DATA_PACK "Blocks.png", true)) return false;

	font->SetColor(LIBC_BLACK);
	
	// Config reading
	cfg.Load("Tetris.cfg", DATA_PACK "Tetris.cfg");
	keyDelay = cfg.GetFloat("KeyDelay", 0.15f);
	ghostEnabled = cfg.GetBool("ghostEnabled", false);

	// Record reading
	libFile f("Record.sav", LIB_FILE_READ_BIN);
	f.Read(&record, sizeof(libHolder), 1);
	f.Close();

	NewGame();

	return true;
}

/*
===================
Glass::Free
===================
*/
void Glass::Free()
{
	SaveRecord();
	cfg.Save();
	cfg.Unload();

	engine->Free_libFont(font);
	engine->Free_libSound(drop);
	engine->Free_libTexture(blocks);
}

/*
===================
Glass::Draw
===================
*/
void Glass::Draw(float x, float y)
{
	if (help)
	{
		// Glass
		engine->Draw2DQuad(libQuad(x, y, WIDTH - BLOCK_WIDTH, HEIGHT - BLOCK_HEIGHT));
		DrawRectangleOutline(libQuad(x, y, WIDTH - BLOCK_WIDTH, HEIGHT - BLOCK_HEIGHT), LIBC_BLACK);

		// Help	
		font->Print2D(80.0f + x, BLOCK_HEIGHT + y - 10.0f, libVA("Tetris 2012 ver. %s", TETRIS_VERSION));
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 2 + y, "Up - Rotate the figure.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 3 + y, "Left - Move the figure left.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 4 + y, "Right - Move the figure right.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 5 + y, "Down - Move the figure down.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 6 + y, "Space - Drop the figure.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 7 + y, "G - Turn on/off the ghost.");
		font->Print2D(10.0f + x, BLOCK_HEIGHT * 8 + y, "F12 - Take a screenshot.");
		font->SetScale(libVec3(0.8f, 0.8f, 0.0f));
		font->Print2D(10.0f + x, HEIGHT - BLOCK_HEIGHT * 3.0f - 20.0f + y, "Copyright (C) 2012 Ilya Lyakhovets");
		font->SetScale(libVec3(1.0f, 1.0f, 0.0f));
	}
	else
	{
		// Glass
		engine->Draw2DQuad(libQuad(x, y, BLOCK_WIDTH * GLASS_WIDTH + x, BLOCK_HEIGHT * GLASS_HEIGHT + y));
		
		// Draw the figure
		if (IsThereFreeSpaceForNewFigure())
		{
			for (int i = 0; i < FIGURE_WIDTH; i++)
			{
				for (int j = 0; j < FIGURE_HEIGHT; j++)
				{
					if (figure.blocks[i][j].filled)
					{
						DrawBlock(BLOCK_WIDTH * (i + figure.x) + x, BLOCK_HEIGHT * (j + figure.y) + y, figure.blocks[i][j].clr);
					}
				}
			}
		}

		// Draw ghosts of the blocks in the glass
		if (ghostEnabled && !gameOver)
		{
			int tempX = figure.x;
			int TempY = figure.y;
		
			while (!IsFigureDropped()) figure.y++;

			for (int i = 0; i < FIGURE_WIDTH; i++)
			{
				for (int j = 0; j < FIGURE_HEIGHT; j++)
				{
					if (figure.blocks[i][j].filled)
					{	
						DrawBlock(BLOCK_WIDTH * (i + figure.x) + x, BLOCK_HEIGHT * (j + figure.y) + y, figure.blocks[i][j].clr, BLOCK_GHOST_COLOR);
					}
				}
			}

			figure.x = tempX;
			figure.y = TempY;
		}

		// Draw blocks in the glass
		for (int i = 0; i < GLASS_WIDTH; i++)
		{
			for (int j = 0; j < GLASS_HEIGHT; j++)
			{
				if (glass[i][j].filled)
				{
					DrawBlock(BLOCK_WIDTH * i + x, BLOCK_HEIGHT * j + y, glass[i][j].clr);
				}
			}
		}

		DrawRectangleOutline(libQuad(x, y, BLOCK_WIDTH * GLASS_WIDTH + x, BLOCK_HEIGHT * GLASS_HEIGHT + y), LIBC_BLACK);

		// A Cell for a next new figure
		engine->Draw2DQuad(libQuad(BLOCK_WIDTH * GLASS_WIDTH + x + 32.0f, y, BLOCK_WIDTH * GLASS_WIDTH + BLOCK_WIDTH * FIGURE_WIDTH + x + 32.0f, BLOCK_HEIGHT * FIGURE_HEIGHT + y + 32.0f));

		// Draw a next new figure
		for (int i = 0; i < FIGURE_WIDTH; i++)
		{
			for (int j = 0; j < FIGURE_HEIGHT; j++)
			{
				if (g_figures[figure.numNext][0][j][i])
				{
					DrawBlock(320.0f + BLOCK_WIDTH * i + x + 32.0f, BLOCK_HEIGHT * j + y + 32.0f, (color_t) figure.numNext);
				}
			}
		}

		DrawRectangleOutline(libQuad(320.0f + x + 32.0f, y, 448 + x + 32.0f, 32.0f + y), LIBC_BLACK);
		DrawRectangleOutline(libQuad(320.0f + x + 32.0f, 32.0f + y, 448.0f + x + 32.0f, BLOCK_HEIGHT * FIGURE_HEIGHT + y + 32.0f), LIBC_BLACK);

		// Cells
		for (int i = 0; i < 4; i++)
		{
			engine->Draw2DQuad(libQuad(320.0f + x + 32.0f, 190.0f + y + 96.0f * i, 448.0f + x + 32.0f, 254.0f + y + 96.0f * i));
			DrawRectangleOutline(libQuad(320.0f + x + 32.0f, 190.0f + y + 96.0f * i, 448.0f + x + 32.0f, 222.0f + y + 96.0f * i), LIBC_BLACK);
			DrawRectangleOutline(libQuad(320.0f + x + 32.0f, 222.0f + y + 96.0f * i, 448.0f + x + 32.0f, 254.0f + y + 96.0f * i), LIBC_BLACK);
		}

		font->Print2D(320.0f + x + 32.0f, y, "Next");
		font->Print2D(320.0f + x + 32.0f, 190.0f + y, "Score");
		font->Print2D(320.0f + x + 32.0f, 222.0f + y, "%d", score.Get());
		font->Print2D(320.0f + x + 32.0f, 286.0f + y, "Goal");
		font->Print2D(320.0f + x + 32.0f, 318.0f + y, "%d", scoreGoal.Get());
		font->Print2D(320.0f + x + 32.0f, 382.0f + y, "Level");
		font->Print2D(320.0f + x + 32.0f, 412.0f + y, "%d", level.Get());
		font->Print2D(320.0f + x + 32.0f, 476.0f + y, "Record");
		font->Print2D(320.0f + x + 32.0f, 508.0f + y, "%d", record.Get());

		// Draw "game over" table
		if (gameOver)
		{
			engine->Draw2DQuad(libQuad(BLOCK_WIDTH * 2 + x, BLOCK_HEIGHT * 5 + y, BLOCK_WIDTH * 8 + x, BLOCK_HEIGHT * 6 + y));
			DrawRectangleOutline(libQuad(BLOCK_WIDTH * 2 + x, BLOCK_HEIGHT * 5 + y, BLOCK_WIDTH * 8 + x, BLOCK_HEIGHT * 6 + y), LIBC_BLACK);

			if (score.Get() != MAX_SCORE)
				font->Print2D(BLOCK_WIDTH * 2 + BLOCK_WIDTH / 2 + x, BLOCK_HEIGHT * 5 + y, "GAME OVER!");
			else
				font->Print2D(BLOCK_WIDTH * 2 + BLOCK_WIDTH / 2 + x, BLOCK_HEIGHT * 5 + y, "MAX SCORE!");
		}
	}
}

/*
===================
Glass::Update
===================
*/
void Glass::Update()
{
	if (engine->IsKeyDown(LIBK_F1)) help = !help;
	if (engine->IsKeyDown(LIBK_G))
	{
		ghostEnabled = !ghostEnabled;
		cfg.SetBool("ghostEnabled", ghostEnabled);
	}

	if (!gameOver)
	{
		// Move the figure down every the delay time
		if (fallDelay.Get() >= FIGURE_FALL_DELAY - FIGURE_FALL_DELAY_DECREASE * level.Get())
		{
			fallDelay.Restart();
			MoveDown();
		}

		// Move the figure left
		if ((engine->IsKey(LIBK_A) || engine->IsKey(LIBK_LEFT)) && rightFigureDelay.Get() >= keyDelay)
		{
			rightFigureDelay.Restart();
			MoveLeft();
		}
		// Move the figure right
		if ((engine->IsKey(LIBK_D) || engine->IsKey(LIBK_RIGHT)) && rightFigureDelay.Get() >= keyDelay)
		{
			rightFigureDelay.Restart();
			MoveRight();
		}
		// Move the figure down
		if ((engine->IsKey(LIBK_S) || engine->IsKey(LIBK_DOWN)) && downFigureDelay.Get() >= keyDelay)
		{
			fallDelay.Restart();
			downFigureDelay.Restart();
			MoveDown();
		}
		// Rotate the figure
		if (engine->IsKeyDown(LIBK_W) || engine->IsKeyDown(LIBK_UP))
		{
			RotateFigure();
		}
		// Drop the figure
		if (engine->IsKeyDown(LIBK_SPACE))
		{
			fallDelay.Restart();
			DropFigure();
		}

		if (IsFigureDropped())
		{
			BuildFigureIntoGlass();
			RemoveFilledLines();
			drop->Play();

			figure.New();
			figure.x = (GLASS_WIDTH / 2) - (FIGURE_WIDTH / 2);

			if (IsFigureDropped() || !IsThereFreeSpaceForNewFigure())
				gameOver = true;
		}
	}
	else
	{
		// Restart
		if (engine->IsKeyDown(LIBK_SPACE))
		{
			SaveRecord();
			NewGame();
		}
	}
}

/*
===================
Glass::SaveRecord
===================
*/
void Glass::SaveRecord()
{
	if (score.Get() > record.Get())
	{
		record.Set(score);

		libFile f("Record.sav", LIB_FILE_WRITE_BIN);
		f.Write(&record, sizeof(libHolder), 1);
		f.Close();
	}
}

/*
===================
Glass::DrawBlock
===================
*/
void Glass::DrawBlock(float x, float y, color_t clrType, const libClr &clr) const
{
	if (clrType == CYAN)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, x, y, clr);
	else if (clrType == BLUE)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, 0, BLOCK_WIDTH * 2, BLOCK_HEIGHT, x, y, clr);
	else if (clrType == ORANGE)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, 0, BLOCK_WIDTH * 3, BLOCK_HEIGHT, x, y, clr);
	else if (clrType == YELLOW)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT * 2, x, y, clr);
	else if (clrType == GREEN)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT * 2, x, y, clr);
	else if (clrType == PURPLE)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 2, x, y, clr);
	else if (clrType == RED)
		blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT * 2, BLOCK_WIDTH, BLOCK_HEIGHT * 3, x, y, clr);
}

/*
===================
Glass::DrawRectangleOutline
===================
*/
void Glass::DrawRectangleOutline(const libQuad &quad, const libClr &clr) const
{
	engine->Draw2DLine(libVertex(quad.v[0].x, quad.v[0].y, clr), libVertex(quad.v[1].x, quad.v[1].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[1].x, quad.v[1].y, clr), libVertex(quad.v[2].x, quad.v[2].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[2].x, quad.v[2].y, clr), libVertex(quad.v[3].x, quad.v[3].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[3].x, quad.v[3].y, clr), libVertex(quad.v[0].x, quad.v[0].y, clr), 2.0f);
}

/*
===================
Glass:NewGame
===================
*/
void Glass::NewGame()
{
	for (int i = 0; i < GLASS_WIDTH; i++)
		for (int j = 0; j < GLASS_HEIGHT; j++)
			glass[i][j].filled = false;

	figure.New();
	figure.x = (GLASS_WIDTH / 2) - (FIGURE_WIDTH / 2);

	gameOver = false;
	score.Set(0);
	scoreGoal.Set(STARTUP_SCORE_GOAL);
	level.Set(1);

	fallDelay.Restart();
	leftFigureDelay.Restart();
	rightFigureDelay.Restart();
	downFigureDelay.Restart();
}

/*
===================
Glass::BuildFigureIntoGlass
===================
*/
void Glass::BuildFigureIntoGlass()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				glass[figure.x + i][figure.y + j].filled = true;
				glass[figure.x + i][figure.y + j].clr = figure.blocks[i][j].clr;
			}
		}
	}
}

/*
===================
Glass::RemoveFilledLines
===================
*/
void Glass::RemoveFilledLines()
{
	int numOfFilledLines = 0;

	for (int i = GLASS_HEIGHT - 1; i > 0; i--)
	{
		bool filled = true;

		for (int j = 0; j < GLASS_WIDTH; j++)
		{
			if (!glass[j][i].filled)
			{
				filled = false;
			}
		}

		if (filled)
		{
			for (int j = 0; j < GLASS_WIDTH; j++)
				glass[j][i].filled = false;

			for (int len = i; len > 0; len--)
			{
				for (int j = 0; j < GLASS_WIDTH; j++)
				{
					glass[j][len].filled = glass[j][len-1].filled;
					glass[j][len-1].filled = false;
					glass[j][len].clr = glass[j][len-1].clr;
				}
			}

			i++;
			numOfFilledLines++;
		}
	}

	if (numOfFilledLines)
	{
		if (numOfFilledLines == 1)
			score.Set(score.Get() + SCORE_FOR_1_LINE * level.Get());
		else if (numOfFilledLines == 2)
			score.Set(score.Get() + SCORE_FOR_2_LINES * level.Get());
		else if (numOfFilledLines == 3)
			score.Set(score.Get() + SCORE_FOR_3_LINES * level.Get());
		else
			score.Set(score.Get() + SCORE_FOR_4_LINES * level.Get());

		if (score.Get() >= scoreGoal.Get())
		{
			if (level.Get() == NUMBER_OF_LEVELS)
			{
				score.Set(MAX_SCORE);
				scoreGoal.Set(MAX_SCORE);
				gameOver = true;
				return;
			}

			level.Set(level.Get() + 1);
			scoreGoal.Set(scoreGoal.Get() * SCORE_GOAL_MULTIPLIER);
		}
	}
}

/*
===================
Glass::IsFigureDropped
===================
*/
bool Glass::IsFigureDropped() const
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				if (figure.blocks[i][j + 1].filled && glass[figure.x + i][figure.y + j + 1].filled)
					continue;

				if (figure.y + j + 1 == GLASS_HEIGHT || glass[figure.x + i][figure.y + j + 1].filled)
					return true;
			}
		}
	}

	return false;
}

/*
===================
Glass::IsThereFreeSpaceForNewFigure
===================
*/
bool Glass::IsThereFreeSpaceForNewFigure() const
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
		for (int j = 0; j < FIGURE_HEIGHT; j++)
			if (figure.blocks[i][j].filled && glass[figure.x + i][figure.y + j].filled)
				return false;

	return true;
}

/*
===================
Glass::MoveLeft
===================
*/
void Glass::MoveLeft()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.x + i <= 0 && figure.blocks[i][j].filled)
				return;

			if (figure.blocks[i][j].filled && glass[figure.x + i - 1][figure.y + j].filled && !figure.blocks[i - 1][j].filled)
				return;
		}
	}

	figure.x--;
}

/*
===================
Glass::MoveRight
===================
*/
void Glass::MoveRight()
{
	for (int i = FIGURE_WIDTH - 1; i >= 0; i--)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.x + i >= GLASS_WIDTH - 1 && figure.blocks[i][j].filled)
				return;

			if (figure.blocks[i][j].filled && glass[figure.x + i + 1][figure.y + j].filled && !figure.blocks[i + 1][j].filled)
				return;
		}
	}

	figure.x++;
}

/*
===================
Glass::MoveDown
===================
*/
void Glass::MoveDown()
{
	if (!IsFigureDropped()) figure.y++;
}

/*
===================
Glass::DropFigure
===================
*/
void Glass::DropFigure()
{
	while (!IsFigureDropped()) MoveDown();
}

/*
===================
Glass::RotateFigure
===================
*/
void Glass::RotateFigure()
{
	bool flag;
	bool allowRotate = true;
	int xOld = figure.x;

	int rot = figure.rot + 1;
	if (rot == ROTATIONS_OF_FIGURE) rot = 0;

	// Move the figure right from wall if there is not enough free space to rotate
	do
	{
		flag = true;

		for (int i = 0; i < -figure.x; i++)
		{
			for (int j = 0; j < FIGURE_HEIGHT; j++)
			{
				if (g_figures[figure.num][rot][j][i])
				{
					flag = false;
				}
			}
		}
	
		if (!flag) MoveRight();
	} while (!flag);

	// Move the figure left from wall if there is not enough free space to rotate
	do
	{
		flag = true;

		for (int i = FIGURE_WIDTH - 1; i > GLASS_WIDTH - 1 - figure.x; i--)
		{
			for (int j = 0; j < FIGURE_HEIGHT; j++)
			{
				if (g_figures[figure.num][rot][j][i])
				{
					flag = false;
				}
			}
		}

		if (!flag) MoveLeft();
	} while (!flag);

	// Don't allow to rotate the figure when it's too low
	for (int i = FIGURE_HEIGHT - 1; i >= 0; i--)
		for (int j = 0; j < FIGURE_WIDTH; j++)
			if (g_figures[figure.num][rot][j][i] && figure.y + j >= GLASS_HEIGHT)
				allowRotate = false;

	// Don't allow to rotate the figure if there are any other blocks
	for (int i = 0; i < FIGURE_WIDTH; i++)
		for (int j = 0; j < FIGURE_HEIGHT; j++)
			if (g_figures[figure.num][rot][j][i] & glass[figure.x + i][figure.y + j].filled & !g_figures[figure.num][figure.rot][j][i])
				allowRotate = false;

	if (allowRotate)
		figure.Rotate();
	else
		figure.x = xOld;
}
