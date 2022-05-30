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

#include "Main.h"
#include "Game.h"

/*
===================
Game::Game
===================
*/
Game::Game()
{
	help = false;
	font = nullptr;
	drop = nullptr;
}

/*
===================
Game::Init
===================
*/
bool Game::Init()
{
	if (!engine->GetFont(font, DATA_PACK "Font.ttf")) return false;
	if (!engine->GetTexture(blocks, DATA_PACK "Blocks.png")) return false;
	if (!engine->GetSound(drop, DATA_PACK "Drop.wav")) return false;

	font->SetColor(LIB_COLOR_BLACK);
	font->SetSize(18);
	
	cfg.Load("Tetris.cfg", DATA_PACK "Tetris.cfg");
	keyDelay = cfg.GetFloat("KeyDelay", DEFAULT_KEY_DELAY);
	engine->SetState(LIB_AUDIO_VOLUME, (float) cfg.GetBool("soundEnabled", DEFAULT_SOUND_ENABLED));
	helperEnabled = cfg.GetBool("helperEnabled", DEFAULT_HELPER_ENABLED);

	libFile f("Record.sav", libFile::Mode::ReadBin);
	f.Read(&record, sizeof(libHolder), 1);
	f.Close();

	NewGame();

	return true;
}

/*
===================
Game::Draw
===================
*/
void Game::Draw()
{
	const float x = 32.0f;
	const float y = 32.0f;

	// Help screen
	if (help)
	{
		engine->Draw2DQuad(libQuad(libVertex(BLOCK_SIZE, BLOCK_SIZE), libVertex(WIDTH - BLOCK_SIZE, HEIGHT - BLOCK_SIZE)));
		DrawBorder(libQuad(libVertex(BLOCK_SIZE, BLOCK_SIZE), libVertex(WIDTH - BLOCK_SIZE, HEIGHT - BLOCK_SIZE)), LIB_COLOR_BLACK);

		font->SetAlign(LIB_HCENTER | LIB_TOP);
		font->SetSize(28);
		font->Print2D(WIDTH / 2, y + 20, libFormat("Tetris 2012 ver. %s\n", TETRIS_VERSION));

		float lineSpacing = font->GetLineSpacing();
		font->SetSize(18);
		font->Print2D(WIDTH / 2, y + 20.0f + lineSpacing,
			"W / Up - Rotate the figure.\n"
			"A / Left - Move the figure left.\n"
			"D / Right - Move the figure right.\n"
			"S / Down - Move the figure down.\n"
			"Space - Drop the figure.\n"
			"T - Mute/unmute sound.\n"
			"H - Turn the figure helper on/off.\n"
			"F12 - Take a screenshot.");

		font->SetAlign(LIB_HCENTER | LIB_BOTTOM);
		font->SetSize(16);
		font->Print2D(WIDTH / 2, HEIGHT - 45.0f, "Copyright (C) 2012 Ilya Lyakhovets");
	}
	// Game screen
	else
	{
		// Game field
		engine->Draw2DQuad(libQuad(libVertex(x, y), libVertex(BLOCK_SIZE * FIELD_WIDTH + x, BLOCK_SIZE * FIELD_HEIGHT + y)));

		// The current figure
		if (IsThereSpaceForNewFigure())
		{
			for (int i = 0; i < FIGURE_WIDTH; i++)
			{
				for (int j = 0; j < FIGURE_HEIGHT; j++)
				{
					if (figure.blocks[i][j].filled)
					{
						DrawBlock(BLOCK_SIZE * (i + figure.x) + x, BLOCK_SIZE * (j + figure.y) + y, figure.blocks[i][j].color);
					}
				}
			}
		}

		// The figure helper
		if (helperEnabled && !gameOver)
		{
			int tempX = figure.x;
			int tempY = figure.y;

			while (!IsFigureDropped()) figure.y++;

			// Shows only if there is enough space
			if (figure.y - FIGURE_HEIGHT > tempY)
			{
				for (int i = 0; i < FIGURE_WIDTH; i++)
				{
					for (int j = 0; j < FIGURE_HEIGHT; j++)
					{
						if (figure.blocks[i][j].filled)
						{
							DrawBlock(BLOCK_SIZE * (i + figure.x) + x, BLOCK_SIZE * (j + figure.y) + y, figure.blocks[i][j].color, HELPER_COLOR);
						}
					}
				}
			}

			figure.x = tempX;
			figure.y = tempY;
		}

		// Blocks in the field
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			for (int j = 0; j < FIELD_HEIGHT; j++)
			{
				if (field[i][j].filled)
				{
					DrawBlock(x + BLOCK_SIZE * i, y + BLOCK_SIZE * j, field[i][j].color);
				}
			}
		}

		// Game border
		DrawBorder(libQuad(libVertex(x, y), libVertex(x + BLOCK_SIZE * FIELD_WIDTH, y + BLOCK_SIZE * FIELD_HEIGHT)), LIB_COLOR_BLACK);

		// Next figure
		engine->Draw2DQuad(libQuad(libVertex(WIDTH - BLOCK_SIZE - BLOCK_SIZE * FIGURE_WIDTH, y), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE + BLOCK_SIZE * FIGURE_HEIGHT)));

		for (int i = 0; i < FIGURE_WIDTH; i++)
		{
			for (int j = 0; j < FIGURE_HEIGHT; j++)
			{
				if (g_figures[figure.numNext][0][j][i])
				{
					DrawBlock(float(WIDTH - BLOCK_SIZE - BLOCK_SIZE * FIGURE_WIDTH + BLOCK_SIZE * i), y + BLOCK_SIZE + BLOCK_SIZE * j, (Block::Color)figure.numNext);
				}
			}
		}

		DrawBorder(libQuad(libVertex(WIDTH - BLOCK_SIZE * 5, y), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE)), LIB_COLOR_BLACK);
		DrawBorder(libQuad(libVertex(WIDTH - BLOCK_SIZE * 5, y + BLOCK_SIZE), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE + BLOCK_SIZE * FIGURE_HEIGHT)), LIB_COLOR_BLACK);

		// Other panels
		for (int i = 0; i < 4; i++)
		{
			engine->Draw2DQuad(libQuad(libVertex(WIDTH - BLOCK_SIZE * 5, y + BLOCK_SIZE * 6 + BLOCK_SIZE * 3 * i), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE * 8 + BLOCK_SIZE * 3 * i)));
			DrawBorder(libQuad(libVertex(WIDTH - BLOCK_SIZE * 5, y + BLOCK_SIZE * 6 + BLOCK_SIZE * 3 * i), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE * 7 + BLOCK_SIZE * 3 * i)), LIB_COLOR_BLACK);
			DrawBorder(libQuad(libVertex(WIDTH - BLOCK_SIZE * 5, y + BLOCK_SIZE * 7 + BLOCK_SIZE * 3 * i), libVertex(WIDTH - BLOCK_SIZE, y + BLOCK_SIZE * 8 + BLOCK_SIZE * 3 * i)), LIB_COLOR_BLACK);
		}

		font->SetSize(18);
		font->SetAlign(LIB_CENTER);
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE / 2, "Next");
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 6 + BLOCK_SIZE / 2, "Score");
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 7 + BLOCK_SIZE / 2, "%d", score.Get());
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 9 + BLOCK_SIZE / 2, "Goal");
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 10 + BLOCK_SIZE / 2, "%d", scoreGoal.Get());
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 12 + BLOCK_SIZE / 2, "Level");
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 13 + BLOCK_SIZE / 2, "%d", level.Get());
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 15 + BLOCK_SIZE / 2, "Record");
		font->Print2D(WIDTH - BLOCK_SIZE * 3, y + BLOCK_SIZE * 16 + BLOCK_SIZE / 2, "%d", record.Get());

		// Game over message
		if (gameOver)
		{
			libQuad gameOverPanel(libVertex(x + BLOCK_SIZE, y + BLOCK_SIZE * 5), libVertex(x + BLOCK_SIZE * 9, y + BLOCK_SIZE * 7));
			engine->Draw2DQuad(gameOverPanel);
			DrawBorder(gameOverPanel, LIB_COLOR_BLACK);

			if (score.Get() != MAX_SCORE)
				font->Print2D(x + FIELD_WIDTH * BLOCK_SIZE / 2, y + BLOCK_SIZE * 6, "GAME OVER!");
			else
				font->Print2D(x + FIELD_WIDTH * BLOCK_SIZE / 2, y + BLOCK_SIZE * 6, "MAX SCORE!");
		}
	}
}

/*
===================
Game::Update
===================
*/
void Game::Update()
{
	if (engine->IsKeyPressed(LIBK_F1)) help = !help;

	// Turns on/off sound
	if (engine->IsKeyPressed(LIBK_T))
	{
		bool soundEnabled = !cfg.GetBool("soundEnabled", true);

		engine->SetState(LIB_AUDIO_VOLUME, soundEnabled ? 1.0f : 0.0f);
		cfg.SetBool("soundEnabled", soundEnabled);
	}

	// Turns on/off the figure helper
	if (engine->IsKeyPressed(LIBK_H))
	{
		helperEnabled = !helperEnabled;
		cfg.SetBool("helperEnabled", helperEnabled);
	}

	// Game logic
	if (!gameOver)
	{
		// Moves the figure down based on fall delay
		if (fallDelay.Get() >= FIGURE_FALL_START_DELAY / (1.0f + ((level.Get() - 1) * FIGURE_FALL_DECREASE_COEFF)))
		{
			fallDelay.Restart();
			MoveDown();
		}

		// Moves the figure left
		if ((engine->IsKeyPressed(LIBK_A) || engine->IsKeyPressed(LIBK_LEFT)) ||
			(engine->IsKey(LIBK_A) || engine->IsKey(LIBK_LEFT)) && leftFigureDelay.Get() >= keyDelay)
		{
			leftFigureDelay.Restart();
			MoveLeft();
		}
		// Moves the figure right
		if ((engine->IsKeyPressed(LIBK_D) || engine->IsKeyPressed(LIBK_RIGHT)) ||
			(engine->IsKey(LIBK_D) || engine->IsKey(LIBK_RIGHT)) && (rightFigureDelay.Get() >= keyDelay))
		{
			rightFigureDelay.Restart();
			MoveRight();
		}
		// Moves the figure down
		if ((engine->IsKeyPressed(LIBK_S) || engine->IsKeyPressed(LIBK_DOWN)) ||
			(engine->IsKey(LIBK_S) || engine->IsKey(LIBK_DOWN)) && downFigureDelay.Get() >= keyDelay)
		{
			fallDelay.Restart();
			downFigureDelay.Restart();
			MoveDown();
		}
		// Rotates the figure
		if (engine->IsKeyPressed(LIBK_W) || engine->IsKeyPressed(LIBK_UP))
		{
			RotateFigure();
		}
		// Drops the figure
		if (engine->IsKeyPressed(LIBK_SPACE))
		{
			fallDelay.Restart();
			DropFigure();
		}

		if (IsFigureDropped())
		{
			// Adds figure blocks in the field
			for (int i = 0; i < FIGURE_WIDTH; i++)
			{
				for (int j = 0; j < FIGURE_HEIGHT; j++)
				{
					if (figure.blocks[i][j].filled)
					{
						field[figure.x + i][figure.y + j].filled = true;
						field[figure.x + i][figure.y + j].color = figure.blocks[i][j].color;
					}
				}
			}

			RemoveFilledLines();
			drop->Play();

			figure.New();
			figure.x = (FIELD_WIDTH / 2) - (FIGURE_WIDTH / 2);

			if (IsFigureDropped() || !IsThereSpaceForNewFigure())
				gameOver = true;
		}
	}
	else
	{
		// Restart
		if (engine->IsKeyPressed(LIBK_SPACE))
		{
			SaveRecord();
			NewGame();
		}
	}
}

/*
===================
Game::Free
===================
*/
void Game::Free()
{
	SaveRecord();
	cfg.Save();
	cfg.Unload();

	engine->FreeFont(font);
	engine->FreeSound(drop);
	engine->FreeTexture(blocks);
}

/*
===================
Game::SaveRecord
===================
*/
void Game::SaveRecord()
{
	if (score.Get() > record.Get())
	{
		record.Set(score);

		libFile f("Record.sav", libFile::Mode::WriteBin);
		f.Write(&record, sizeof(libHolder), 1);
		f.Close();
	}
}

/*
===================
Game::DrawBlock
===================
*/
void Game::DrawBlock(float x, float y, Block::Color colorType, const libColor &clr) const
{
	int coords[7][4] = { {0, 0, BLOCK_SIZE, BLOCK_SIZE},
						 {BLOCK_SIZE, 0, BLOCK_SIZE * 2, BLOCK_SIZE},
 						 {BLOCK_SIZE * 2, 0, BLOCK_SIZE * 3, BLOCK_SIZE },
						 {0, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE * 2},
						 {BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE * 2, BLOCK_SIZE * 2},
						 {BLOCK_SIZE * 2, BLOCK_SIZE, BLOCK_SIZE * 3, BLOCK_SIZE * 2},
						 {0, BLOCK_SIZE * 2, BLOCK_SIZE, BLOCK_SIZE * 3} };

	blocks->Draw2DAtlas(0.0f, 0.0f, BLOCK_SIZE, BLOCK_SIZE, coords[colorType][0], coords[colorType][1], coords[colorType][2], coords[colorType][3], x, y, clr);
}

/*
===================
Game::DrawBorder
===================
*/
void Game::DrawBorder(const libQuad &quad, const libColor &clr) const
{
	engine->Draw2DLine(libVertex(quad.v[0].x, quad.v[0].y, clr), libVertex(quad.v[1].x, quad.v[1].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[1].x, quad.v[1].y, clr), libVertex(quad.v[2].x, quad.v[2].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[2].x, quad.v[2].y, clr), libVertex(quad.v[3].x, quad.v[3].y, clr), 2.0f);
	engine->Draw2DLine(libVertex(quad.v[3].x, quad.v[3].y, clr), libVertex(quad.v[0].x, quad.v[0].y, clr), 2.0f);
}

/*
===================
Game:NewGame
===================
*/
void Game::NewGame()
{
	for (int i = 0; i < FIELD_WIDTH; i++)
		for (int j = 0; j < FIELD_HEIGHT; j++)
			field[i][j].filled = false;

	figure.New();
	figure.x = (FIELD_WIDTH / 2) - (FIGURE_WIDTH / 2);

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
Game::RemoveFilledLines
===================
*/
void Game::RemoveFilledLines()
{
	int numOfFilledLines = 0;

	for (int i = FIELD_HEIGHT - 1; i > 0; i--)
	{
		bool filled = true;

		for (int j = 0; j < FIELD_WIDTH; j++)
		{
			if (!field[j][i].filled)
			{
				filled = false;
			}
		}

		if (filled)
		{
			for (int j = 0; j < FIELD_WIDTH; j++)
				field[j][i].filled = false;

			for (int len = i; len > 0; len--)
			{
				for (int j = 0; j < FIELD_WIDTH; j++)
				{
					field[j][len].filled = field[j][len-1].filled;
					field[j][len-1].filled = false;
					field[j][len].color = field[j][len-1].color;
				}
			}

			i++;
			numOfFilledLines++;
		}
	}

	// Scoring
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

		if (score.Get() >= MAX_SCORE)
		{
			score.Set(MAX_SCORE);
			gameOver = true;
		}
		else if (score.Get() >= scoreGoal.Get())
		{
			level.Set(level.Get() + 1);
			scoreGoal.Set(scoreGoal.Get() * SCORE_GOAL_MULTIPLIER);
			if (scoreGoal.Get() > MAX_SCORE) scoreGoal.Set(MAX_SCORE);
		}
	}
}

/*
===================
Game::IsFigureDropped
===================
*/
bool Game::IsFigureDropped() const
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.blocks[i][j].filled)
			{
				if (figure.blocks[i][j + 1].filled && field[figure.x + i][figure.y + j + 1].filled)
					continue;

				if (figure.y + j + 1 == FIELD_HEIGHT || field[figure.x + i][figure.y + j + 1].filled)
					return true;
			}
		}
	}

	return false;
}

/*
===================
Game::IsThereSpaceForNewFigure
===================
*/
bool Game::IsThereSpaceForNewFigure() const
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
		for (int j = 0; j < FIGURE_HEIGHT; j++)
			if (figure.blocks[i][j].filled && field[figure.x + i][figure.y + j].filled)
				return false;

	return true;
}

/*
===================
Game::MoveLeft
===================
*/
void Game::MoveLeft()
{
	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.x + i <= 0 && figure.blocks[i][j].filled)
				return;

			if (figure.blocks[i][j].filled && field[figure.x + i - 1][figure.y + j].filled)
				return;
		}
	}

	figure.x--;
}

/*
===================
Game::MoveRight
===================
*/
void Game::MoveRight()
{
	for (int i = FIGURE_WIDTH - 1; i >= 0; i--)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			if (figure.x + i >= FIELD_WIDTH - 1 && figure.blocks[i][j].filled)
				return;

			if (figure.blocks[i][j].filled && field[figure.x + i + 1][figure.y + j].filled)
				return;
		}
	}

	figure.x++;
}

/*
===================
Game::MoveDown
===================
*/
void Game::MoveDown()
{
	if (!IsFigureDropped()) figure.y++;
}

/*
===================
Game::DropFigure
===================
*/
void Game::DropFigure()
{
	while (!IsFigureDropped()) MoveDown();
}

/*
===================
Game::RotateFigure
===================
*/
void Game::RotateFigure()
{
	bool flag;
	bool allowRotate = true;
	int xOld = figure.x;

	int rot = figure.rot + 1;
	if (rot == ROTATIONS_OF_FIGURE) rot = 0;

	// Moves the figure to the right from the wall if there is not enough free space for rotations
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

	// Moves the figure to the left from the wall if there is not enough free space for rotations
	do
	{
		flag = true;

		for (int i = FIGURE_WIDTH - 1; i > FIELD_WIDTH - 1 - figure.x; i--)
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

	for (int i = 0; i < FIGURE_WIDTH; i++)
	{
		for (int j = 0; j < FIGURE_HEIGHT; j++)
		{
			// Doesn't allow rotations of the figure when it's too low
			if (g_figures[figure.num][rot][j][i] && figure.y + j >= FIELD_HEIGHT)
				allowRotate = false;

			// Doesn't allow rotations of the figure if there are any other blocks
			if (g_figures[figure.num][rot][j][i] & field[figure.x + i][figure.y + j].filled & !g_figures[figure.num][figure.rot][j][i])
				allowRotate = false;
		}
	}

	if (allowRotate)
		figure.Rotate();
	else
		figure.x = xOld;
}
