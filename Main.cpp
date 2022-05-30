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
#include "Figure.h"

libEngine *engine = nullptr;
libTexture *background = nullptr;

Game game;

/*
===================
Init
===================
*/
bool Init()
{
	if (!engine->GetTexture(background, DATA_PACK "Background.png")) return false;
	if (!game.Init()) return false;
	
	return true;
}

/*
===================
Render
===================
*/
bool Render()
{
	engine->GfxClear(LIB_COLOR_BLACK);

	background->Draw2DQuad(libQuad(libVertex(0.0f, 0.0f, 0.0f, 0.0f), libVertex(WIDTH, HEIGHT, float(WIDTH) / float(background->GetWidth()), float(HEIGHT) / float(background->GetHeight()))));
	game.Draw();
	
	return true;
}

/*
===================
Frame
===================
*/
bool Frame()
{
	if (engine->IsKeyPressed(LIBK_ESCAPE)) return false;
	if (engine->IsKeyPressed(LIBK_F12)) engine->SystemScreenshot();

	game.Update();

	return true;
}

/*
===================
Free
===================
*/
bool Free()
{
	game.Free();

	return true;
}

/*
===================
libMain
===================
*/
libMain()
{
	if (libGetEngine(engine, ENGINE_PATH))
	{
		engine->SetState(LIB_TITLE, "Tetris 2012");
		engine->SetState(LIB_RESOLUTION, WIDTH, HEIGHT);
		engine->SetState(LIB_RESIZABLE_WINDOW, false);
		engine->SetState(LIB_LOG_FILE, true);
		engine->SetState(LIB_LOG_FILENAME, "Tetris.log");
		engine->SetState(LIB_INIT, Init);
		engine->SetState(LIB_RENDER, Render);
		engine->SetState(LIB_FRAME, Frame);
		engine->SetState(LIB_FREE, Free);

		engine->StartEngine();
		libFreeEngine(engine);
	}
	else
	{
		libDialog::Error("Error!", "Couldn't load libEngine.");
		return -1;
	}

	return 0;
}
