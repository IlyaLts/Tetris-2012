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

libTexture *t_background = nullptr;

Game game;

/*
===================
Init
===================
*/
bool Init()
{
    LIB_CHECK(engine->GetTexture(t_background, DATA_PACK "Background.png"));
    LIB_CHECK(game.Init());
 
    t_background->SetWrapMode(LIB_REPEAT);

    return true;
}

/*
===================
Render
===================
*/
bool Render()
{
    engine->ClearScreen(LIB_COLOR_BLACK);

    float tx = libCast<float>(WIDTH / t_background->GetWidth());
    float ty = libCast<float>(HEIGHT / t_background->GetHeight());
    libQuad q_bg(libVertex(), libVertex(WIDTH, HEIGHT, tx, ty));

    t_background->Draw2DQuad(q_bg);
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
    if (engine->IsKeyPressed(LIBK_ESCAPE))
        return false;

    if (engine->IsKeyPressed(LIBK_F12))
        engine->TakeScreenshot();

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
    if (!libGetEngine(ENGINE_PATH))
    {
        libDialog::Error("Error!", "Couldn't load libEngine.");
        return -1;
    }

    engine->SetState(LIB_WINDOW_TITLE, "Tetris 2012");
    engine->SetState(LIB_WINDOW_SIZE, WIDTH, HEIGHT);
    engine->SetState(LIB_WINDOW_RESIZABLE, false); 
    engine->SetState(LIB_LOG_FILE, true);
    engine->SetState(LIB_LOG_FILENAME, "Tetris.log");
    engine->SetState(LIB_INIT, Init);
    engine->SetState(LIB_RENDER, Render);
    engine->SetState(LIB_FRAME, Frame);
    engine->SetState(LIB_FREE, Free);

    engine->Start();
    libFreeEngine();

    return 0;
}
