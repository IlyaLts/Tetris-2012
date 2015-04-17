// Main.cpp

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
#include "Figure.h"

libEngine *engine		= nullptr;
libTexture *background	= nullptr;

Glass gameGlass;

/*
===================
Init
===================
*/
bool Init()
{
	engine->Get_libTexture(background, DATA_PACK "Background.png", true);
	gameGlass.Init();
	
	return true;
}

/*
===================
Render
===================
*/
bool Render()
{
	engine->GfxClear(LIBC_BLACK);

	background->Draw2DQuad(libQuad(0.0f, 0.0f, 0.0f, 0.0f, WIDTH, HEIGHT, (float) WIDTH / background->GetWidth(), (float) HEIGHT / background->GetHeight()));
	gameGlass.Draw(32.0f, 32.0f);

	return true;
}

/*
===================
Frame
===================
*/
bool Frame()
{
	if (engine->IsKeyDown(LIBK_ESCAPE))
	{
		gameGlass.SaveRecord();
		return false;
	}

	if (engine->IsKeyDown(LIBK_F12)) engine->SysScreenshot("Screenshots/");

	gameGlass.Update();

#ifdef LIB_DEBUG
	engine->SetState(LIB_TITLE, libVA("Tetris FPS: %d", engine->GetFPS()));
#endif

	return true;
}

/*
===================
Free
===================
*/
bool Free()
{
	gameGlass.Free();

	return true;
}

/*
===================
libMain
===================
*/
libMain()
{
	if (libGetEngine(engine))
	{
		engine->SetState(LIB_TITLE, "Tetris");
		engine->SetState(LIB_CLASS_NAME, "Tetris");
		engine->SetState(LIB_RESOLUTION, WIDTH, HEIGHT);
		engine->SetState(LIB_RESIZABLE, false);
		engine->SetState(LIB_FPS_LIMIT, 60);
		engine->SetState(LIB_LOG_FILENAME, "Tetris.log");
		engine->SetState(LIB_LOGFILE, true);
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
