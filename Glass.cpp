// Glass.cpp

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
	fnt = nullptr;
	drop = nullptr;
}

/*
===================
Glass::Init
===================
*/
void Glass::Init()
{
	engine->Get_libFont(fnt, DATA_PACK "Font.png", true);
	engine->Get_libSound(drop, DATA_PACK "Drop.wav", false, true);
	engine->Get_libTexture(blocks, DATA_PACK "Blocks.png", true);

	fnt->SetColor(libClr(LIBC_BLACK));
	
	// Config reading
	cfg.Load("Tetris.cfg", DATA_PACK "Tetris.cfg");
	keyDelay = cfg.GetFloat("KeyDelay", 0.15f);
	ghostEnabled = cfg.GetBool("ghostEnabled", false);

	// Record reading
	libFile f("Record.sav", LIB_FILE_READ_BIN);
	f.Read(&record, sizeof(libHolder), 1);
	f.Close();

	NewGame();
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

	engine->Free_libFont(fnt);
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
		engine->Draw2DQuad(libQuad(x, y, WIDTH - x, HEIGHT - y));
		engine->Draw2DLine(libVertex(x, HEIGHT - y, LIBC_BLACK), libVertex(WIDTH - x, HEIGHT - y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(WIDTH - x, HEIGHT - y, LIBC_BLACK), libVertex(WIDTH-x, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(WIDTH - x, y, LIBC_BLACK), libVertex(x, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(x, y, LIBC_BLACK), libVertex(x, HEIGHT - y, LIBC_BLACK), 2.0f);

		// Help
		fnt->Print2D(150.0f + x, BLOCK_HEIGHT + y - 10.0f, libVA("Tetris %s", TETRIS_VERSION));
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 2 + y, "Up - Rotate the figure.");
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 3 + y, "Left/Right - Move the figure.");
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 4 + y, "Down - Down the figure.");
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 5 + y, "Space - Drop the figure.");
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 6 + y, "G - Turn on/off the ghost.");
		fnt->Print2D(10.0f + x, BLOCK_HEIGHT * 7 + y, "F12 - Take a screenshot.");
		fnt->SetScale(0.8f, 0.8f);
		fnt->Print2D(10.0f + x, HEIGHT - BLOCK_HEIGHT * 3.0f - 20.0f + y, "Copyright (C) 2012 Ilya Lyakhovets");
		fnt->SetScale(1.0f, 1.0f);
	}
	else
	{
		// Glass
		engine->Draw2DQuad(libQuad(x, y, BLOCK_WIDTH * GLASS_WIDTH + x, HEIGHT - y));
		
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
						float fx = BLOCK_WIDTH * (i + figure.x) + x;
						float fy = BLOCK_HEIGHT * (j + figure.y) + y;
						
						if (figure.blocks[i][j].clr == CYAN)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == BLUE)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, 0, BLOCK_WIDTH * 2, BLOCK_HEIGHT, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == ORANGE)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, 0, BLOCK_WIDTH * 3, BLOCK_HEIGHT, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == YELLOW)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT * 2, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == GREEN)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT * 2, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == PURPLE)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 2, fx, fy, BLOCK_GHOST_ALPHA);
						else if (figure.blocks[i][j].clr == RED)
							blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT * 2, BLOCK_WIDTH, BLOCK_HEIGHT * 3, fx, fy, BLOCK_GHOST_ALPHA);
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
					float fx = BLOCK_WIDTH * i + x;
					float fy = BLOCK_HEIGHT * j + y;

					if (glass[i][j].clr == CYAN)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, fx, fy, libClr());
					else if (glass[i][j].clr == BLUE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, 0, BLOCK_WIDTH * 2, BLOCK_HEIGHT, fx, fy);
					else if (glass[i][j].clr == ORANGE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, 0, BLOCK_WIDTH * 3, BLOCK_HEIGHT, fx, fy);
					else if (glass[i][j].clr == YELLOW)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT * 2, fx, fy);
					else if (glass[i][j].clr == GREEN)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT * 2, fx, fy);
					else if (glass[i][j].clr == PURPLE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 2, fx, fy);
					else if (glass[i][j].clr == RED)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT * 2, BLOCK_WIDTH, BLOCK_HEIGHT * 3, fx, fy);
				}
			}
		}

		engine->Draw2DLine(libVertex(x, HEIGHT - y, LIBC_BLACK), libVertex(BLOCK_WIDTH * GLASS_WIDTH + x, HEIGHT - y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(BLOCK_WIDTH * GLASS_WIDTH + x, HEIGHT - y, LIBC_BLACK), libVertex(BLOCK_WIDTH * GLASS_WIDTH + x, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(BLOCK_WIDTH * GLASS_WIDTH + x, y, LIBC_BLACK), libVertex(x, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(x, y, LIBC_BLACK), libVertex(x, HEIGHT - y, LIBC_BLACK), 2.0f);

		// A Cell for a next new figure
		engine->Draw2DQuad(libQuad(WIDTH - 32.0f - BLOCK_WIDTH * FIGURE_WIDTH, y, WIDTH - 32.0f, BLOCK_HEIGHT * FIGURE_HEIGHT + y + 32.0f));

		// Draw a next new figure
		for (int i = 0; i < FIGURE_WIDTH; i++)
		{
			for (int j = 0; j < FIGURE_HEIGHT; j++)
			{
				if (g_figures[figure.numNext][0][j][i])
				{
					float fx = 320.0f + BLOCK_WIDTH * i + x + 32.0f;
					float fy = BLOCK_HEIGHT * j + y + 32.0f;

					if (figure.numNext == CYAN)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, fx, fy);
					else if (figure.numNext == BLUE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, 0, BLOCK_WIDTH * 2, BLOCK_HEIGHT, fx, fy);
					else if (figure.numNext == ORANGE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, 0, BLOCK_WIDTH * 3, BLOCK_HEIGHT, fx, fy);
					else if (figure.numNext == YELLOW)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT * 2, fx, fy);
					else if (figure.numNext == GREEN)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT * 2, fx, fy);
					else if (figure.numNext == PURPLE)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_WIDTH * 2, BLOCK_HEIGHT, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 2, fx, fy);
					else if (figure.numNext == RED)
						blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_WIDTH, BLOCK_HEIGHT, 0, BLOCK_HEIGHT * 2, BLOCK_WIDTH, BLOCK_HEIGHT * 3, fx, fy);
				}
			}
		}

		engine->Draw2DLine(libVertex(320.0f + x + 32.0f, 158.0f + y, LIBC_BLACK), libVertex(448.0f + x + 32.0f, 158.0f + y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(448.0f + x + 32.0f, 158.0f + y, LIBC_BLACK), libVertex(448.0f + x + 32.0f, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(448.0f + x + 32.0f, y, LIBC_BLACK), libVertex(320.0f + x + 32.0f, y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(320.0f + x + 32.0f, y, LIBC_BLACK), libVertex(320 + x + 32.0f, 158.0f + y, LIBC_BLACK), 2.0f);
		engine->Draw2DLine(libVertex(320.0f + x + 32.0f, 32.0f + y, LIBC_BLACK), libVertex(448 + x + 32.0f, 32.0f + y, LIBC_BLACK), 2.0f);

		// Cells
		for (int i = 0; i < 4; i++)
		{
			engine->Draw2DQuad(libQuad(320.0f + x + 32.0f, 190.0f + y + 96.0f * i, 448.0f + x + 32.0f, 254.0f + y + 96.0f * i));
			engine->Draw2DLine(libVertex(320.0f + x + 32.0f, 254.0f + y + 96.0f * i, LIBC_BLACK), libVertex(448.0f + x + 32.0f, 254.0f + y + 96.0f * i, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(448.0f + x + 32.0f, 254.0f + y + 96.0f * i, LIBC_BLACK), libVertex(448.0f + x + 32.0f, 190.0f + y + 96.0f * i, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(448.0f + x + 32.0f, 190.0f + y + 96.0f * i, LIBC_BLACK), libVertex(320.0f + x + 32.0f, 190.0f + y + 96.0f * i, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(320.0f + x + 32.0f, 190.0f + y + 96.0f * i, LIBC_BLACK), libVertex(320.0f + x + 32.0f, 254.0f + y + 96.0f * i, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(320.0f + x + 32.0f, 222.0f + y + 96.0f * i, LIBC_BLACK), libVertex(448.0f + x + 32.0f, 222.0f + y + 96.0f * i, LIBC_BLACK), 2.0f);
		}

		fnt->Print2D(320.0f + x + 32.0f, y, "Next");
		fnt->Print2D(320.0f + x + 32.0f, 190.0f + y, "Score");
		fnt->Print2D(320.0f + x + 32.0f, 222.0f + y, "%d", score.Get());
		fnt->Print2D(320.0f + x + 32.0f, 286.0f + y, "Needed");
		fnt->Print2D(320.0f + x + 32.0f, 318.0f + y, "%d", scoreNeeded.Get());
		fnt->Print2D(320.0f + x + 32.0f, 382.0f + y, "Level");
		fnt->Print2D(320.0f + x + 32.0f, 412.0f + y, "%d", level.Get());
		fnt->Print2D(320.0f + x + 32.0f, 476.0f + y, "Record");
		fnt->Print2D(320.0f + x + 32.0f, 508.0f + y, "%d", record.Get());

		// Draw "game over" table
		if (gameOver)
		{
			engine->Draw2DQuad(libQuad(58.0f + x, 168.0f + y, 270.0f + x, 200.0f + y));
			engine->Draw2DLine(libVertex(58.0f + x, 200.0f + y, LIBC_BLACK), libVertex(270.0f + x, 200.0f + y, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(270.0f + x, 200.0f + y, LIBC_BLACK), libVertex(270.0f + x, 168.0f + y, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(270.0f + x, 168.0f + y, LIBC_BLACK), libVertex(58.0f + x, 168.0f + y, LIBC_BLACK), 2.0f);
			engine->Draw2DLine(libVertex(58.0f + x, 168.0f + y, LIBC_BLACK), libVertex(58.0f + x, 200.0f + y, LIBC_BLACK), 2.0f);

			if (score.Get() != MAX_SCORE)
				fnt->Print2D(80.0f + x, 200.0f, "GAME OVER!");
			else
				fnt->Print2D(80.0f + x, 200.0f, "MAX SCORE!");
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
		if (fallDelay.Get() >= delay)
		{
			fallDelay.Restart();
			MoveDown();
		}

		// Move left
		if ((engine->IsKey(LIBK_A) || engine->IsKey(LIBK_LEFT)) && rightFigureDelay.Get() >= keyDelay)
		{
			rightFigureDelay.Restart();
			MoveLeft();
		}
		// Move right
		if ((engine->IsKey(LIBK_D) || engine->IsKey(LIBK_RIGHT)) && rightFigureDelay.Get() >= keyDelay)
		{
			rightFigureDelay.Restart();
			MoveRight();
		}
		// Down figure
		if ((engine->IsKey(LIBK_S) || engine->IsKey(LIBK_DOWN)) && downFigureDelay.Get() >= keyDelay)
		{
			fallDelay.Restart();
			downFigureDelay.Restart();
			MoveDown();
		}
		// Rotate Figure
		if (engine->IsKeyDown(LIBK_W) || engine->IsKeyDown(LIBK_UP))
		{
			RotateFigure();
		}
		// Drop figure
		if (engine->IsKeyDown(LIBK_SPACE))
		{
			fallDelay.Restart();
			DropFigure();
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
Glass:NewGame
===================
*/
void Glass::NewGame()
{
	for (int i = 0; i < GLASS_WIDTH; i++)
	{
		for (int j = 0; j < GLASS_HEIGHT; j++)
		{
			glass[i][j].filled = false;
		}
	}

	figure.New();
	figure.x = (GLASS_WIDTH / 2) - (FIGURE_WIDTH / 2);
	BuildFigure();

	gameOver = false;
	score.Set(0);
	scoreNeeded.Set(STARTUP_SCORE_GOAL);
	level.Set(1);

	delay = KEY_DELAY;
	fallDelay.Restart();
	leftFigureDelay.Restart();
	rightFigureDelay.Restart();
	downFigureDelay.Restart();
}

/*
===================
Glass::BuildFigure

Build the current figure into the glass
===================
*/
void Glass::BuildFigure()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				glass[figure.x+i][figure.y+j].filled = true;
				glass[figure.x+i][figure.y+j].clr = figure.blocks[i][j].clr;
			}
		}
	}
}

/*
===================
Glass::ClearFigure

Clear the current figure from the glass
===================
*/
void Glass::ClearFigure()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				glass[figure.x+i][figure.y+j].filled = false;
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

	for (int i = GLASS_HEIGHT-1; i > 0; i--)
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

		if (score.Get() >= scoreNeeded.Get())
		{
			if (level.Get() == NUMBERS_OF_LEVELS)
			{
				score.Set(MAX_SCORE);
				gameOver = true;
				return;
			}

			level.Set(level.Get() + 1);
			delay -= KEY_DELAY_DECREASE;

			if (level.Get() != NUMBERS_OF_LEVELS)
				scoreNeeded.Set(scoreNeeded.Get() * SCORE_GOAL_MULTIPLIER);
			else
				scoreNeeded.Set(MAX_SCORE);
		}
	}
}

/*
===================
Glass::IsFigureDropped
===================
*/
bool Glass::IsFigureDropped()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				if (figure.blocks[i][j+1].filled && glass[figure.x+i][figure.y+j+1].filled)
					continue;

				if (figure.y+j+1 == GLASS_HEIGHT || glass[figure.x+i][figure.y+j+1].filled)
					return true;
			}
		}
	}

	return false;
}

/*
===================
Glass::IsThereFreeSpaceForFigure
===================
*/
bool Glass::IsThereFreeSpaceForFigure()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled && glass[figure.x+i][figure.y+j].filled)
			{
				return false;
			}
		}
	}

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
			if (figure.blocks[i][j].filled && figure.x + i == 0)
				return;

			if (i != 0)
			{
				if (figure.blocks[i][j].filled && glass[figure.x+i-1][figure.y+j].filled && !figure.blocks[i-1][j].filled)
					return;
			}
			else
			{
				if (figure.blocks[i][j].filled && glass[figure.x+i-1][figure.y+j].filled)
					return;
			}
		}
	}

	ClearFigure();
	figure.x--;
	BuildFigure();
}

/*
===================
Glass::MoveRight
===================
*/
void Glass::MoveRight()
{
	for (int i = FIGURE_WIDTH-1; i >= 0; i--)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled && figure.x + i == GLASS_WIDTH - 1)
				return;

			if (i+1 < 4)
			{
				if (figure.blocks[i][j].filled && glass[figure.x+i+1][figure.y+j].filled && !figure.blocks[i+1][j].filled)
					return;
			}
			else
			{
				if (figure.blocks[i][j].filled && glass[figure.x+i+1][figure.y+j].filled)
					return;
			}
		}
	}

	ClearFigure();
	figure.x++;
	BuildFigure();
}

/*
===================
Glass::MoveDown
===================
*/
void Glass::MoveDown()
{
	if (!IsFigureDropped())
	{
		ClearFigure();
		figure.y++;
		BuildFigure();
	}

	if (IsFigureDropped())
	{
		RemoveFilledLines();
		drop->Play();
		figure.New();
		figure.x = (GLASS_WIDTH / 2) - (FIGURE_WIDTH / 2);

		if (IsThereFreeSpaceForFigure())
			BuildFigure();

		if (IsFigureDropped())
			gameOver = true;
	}
}

/*
===================
Glass::DropFigure
===================
*/
void Glass::DropFigure()
{
	ClearFigure();
	while (!IsFigureDropped()) figure.y++;
	BuildFigure();

	RemoveFilledLines();
	drop->Play();
	figure.New();
	figure.x = (GLASS_WIDTH / 2) - (FIGURE_WIDTH / 2);

	if (IsThereFreeSpaceForFigure())
		BuildFigure();

	if (IsFigureDropped())
		gameOver = true;
}

/*
===================
Glass::RotateFigure
===================
*/
void Glass::RotateFigure()
{
	bool flag;

	int rot = figure.rot + 1;
	if (rot == ROTATIONS_OF_FIGURE) rot = 0;

	do
	{
		flag = true;

		if (figure.x < 0)
		{
			for (int i = 0; i < figure.x * -1; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (g_figures[figure.num][rot][j][i])
					{
						flag = false;
					}
				}
			}
		}

		if (!flag) MoveRight();
	} while (!flag);

	do
	{
		flag = true;

		if (figure.x > GLASS_WIDTH-4)
		{
			for (int i = 3; i >= 10-figure.x; i--)
			{
				for (int j = 0; j < 4; j++)
				{
					if (g_figures[figure.num][rot][j][i])
					{
						flag = false;
					}
				}
			}
		}

		if (!flag) MoveLeft();
	} while (!flag);

	if (figure.y > GLASS_HEIGHT-5)
	{
		for (int i = 3; i >= GLASS_HEIGHT-1-figure.y; i--)
		{
			for (int j = 0; j < 4; j++)
			{
				if (g_figures[figure.num][rot+1][j][i] && glass[figure.x+i][figure.y+j].filled)
				{
					flag = false;
					break;
				}

				if (!flag)
					break;
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (g_figures[figure.num][figure.rot+1][j][i] & glass[figure.x+i][figure.y+j].filled & !g_figures[figure.num][figure.rot][j][i])
			{
				flag = false;
				break;
			}

			if (!flag) break;
		}
	}

	if (flag)
	{
		ClearFigure();
		figure.Rotate();
		BuildFigure();
	}
}
