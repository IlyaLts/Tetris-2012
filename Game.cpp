/*
===============================================================================
    Copyright (C) 2012 Ilya Lyakhovets

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

constexpr float margin = 32.0f;
constexpr float blockSize = 32.0f;

constexpr float tetrominoFallStartDelay = 1.0f;
constexpr float tetrominoFallDecreaseCoeff = 0.4f;

constexpr int scoreSingle = 10;
constexpr int scoreDouble = 25;
constexpr int scoreTriple = 50;
constexpr int scoreQuadruple = 100;
constexpr int startupScoreGoal = 100;
constexpr int scoreGoalMultiplier = 2;
constexpr int maxScore = 99999999;

/*
===================
Game::Game
===================
*/
Game::Game()
{
    helpScreen = false;
}

/*
===================
Game::Init
===================
*/
bool Game::Init()
{
    LIB_CHECK(engine->Get(font, DATA_PACK "Font.ttf"));
    LIB_CHECK(engine->Get(tex_background, DATA_PACK "Background.png"));
    LIB_CHECK(engine->Get(tex_blocks, DATA_PACK "Blocks.png"));
    LIB_CHECK(engine->Get(snd_drop, DATA_PACK "Drop.wav"));

    engine->Get(mesh_background);
    engine->Get(mesh_blocks);
    engine->Get(mesh_panel);
    engine->Get(mesh_border);

    tex_background->SetWrapMode(LIB_REPEAT);

    float tx = libCast<float>(windowWidth / tex_background->Width());
    float ty = libCast<float>(windowHeight / tex_background->Height());
    libQuad q_bg(libVertex(), libVertex(windowWidth, windowHeight, tx, ty));

    mesh_background->Add(q_bg);

    font->SetColor(LIB_COLOR_BLACK);
    font->SetSize(18);

    libStr path;
    GetDataPath(path);
    path.Append("/Tetris.cfg");

    cfg.Load(path.Get(), DATA_PACK "Tetris.cfg");
    keyDelay = cfg.GetFloat("KeyDelay", 0.18f);
    engine->SetState(LIB_AUDIO_VOLUME, libCast<float>(cfg.GetBool("soundEnabled", true)));
    helperEnabled = cfg.GetBool("helperEnabled", false);

    LoadRecord();
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
    engine->Draw(mesh_background, tex_background, true);

    if (blocksMeshUpdate)
    {
        blocksMeshUpdate = false;
        UpdateBlocksMesh();
    }

    if (panelsMeshUpdate)
    {
        panelsMeshUpdate = false;
        UpdatePanelsMesh();
    }

    // Help screen
    if (helpScreen)
    {
        engine->Draw(mesh_panel, nullptr, true);
        engine->Draw(mesh_border, nullptr, true);

        libVec2 pos(windowWidth / 2.0f, margin + 20.0f);

        font->SetAlign(LIB_HCENTER | LIB_TOP);
        font->SetSize(28);
        font->Print2D(pos.x, pos.y, libFormat("Tetris 2012 ver. %s\n", TETRIS_VERSION));
        pos.y += font->LineSpacing() * 2.0f;

        font->SetSize(18);
        font->Print2D(pos.x, pos.y, "W / Up - Rotate tetromino.\n"
                                    "A / Left - Move tetromino left.\n"
                                    "D / Right - Move tetromino right.\n"
                                    "S / Down - Move tetromino down.\n"
                                    "Space - Drop tetromino.\n"
                                    "T - Mute/unmute sound.\n"
                                    "H - Turn tetromino helper on/off.\n"
                                    "F12 - Take a screenshot.");

        font->SetAlign(LIB_HCENTER | LIB_BOTTOM);
        font->SetSize(16);
        font->Print2D(windowWidth / 2, windowHeight - 45.0f, "Copyright (C) 2012 Ilya Lyakhovets");
    }
    // Game screen
    else
    {
        engine->Draw(mesh_panel, nullptr, true);
        engine->Draw(mesh_border, nullptr, true);
        engine->Draw(mesh_blocks, tex_blocks, true);

        // Game over message
        if (gameOver)
        {
            if (score.Get() != maxScore)
                font->Print2D(margin + fieldWidth * blockSize / 2, margin + blockSize * 6, "GAME OVER!");
            else
                font->Print2D(margin + fieldWidth * blockSize / 2, margin + blockSize * 6, "MAX SCORE!");
        }

        libVec2 pos(windowWidth - blockSize * 3, margin + blockSize / 2);

        font->SetSize(18);
        font->SetAlign(LIB_CENTER);
        font->Print2D(pos.x, pos.y, "Next");
        pos.y += blockSize * 6;

        font->Print2D(pos.x, pos.y, "Score");
        pos.y += blockSize;

        font->Print2D(pos.x, pos.y, "%d", score.Get());
        pos.y += blockSize * 2.0f;

        font->Print2D(pos.x, pos.y, "Goal");
        pos.y += blockSize;

        font->Print2D(pos.x, pos.y, "%d", scoreGoal.Get());
        pos.y += blockSize * 2.0f;

        font->Print2D(pos.x, pos.y, "Level");
        pos.y += blockSize;

        font->Print2D(pos.x, pos.y, "%d", level.Get());
        pos.y += blockSize * 2.0f;

        font->Print2D(pos.x, pos.y, "Record");
        pos.y += blockSize;

        font->Print2D(pos.x, pos.y, "%d", record.Get());
    }
}

/*
===================
Game::Update
===================
*/
void Game::Update()
{
    if (engine->IsKeyPressed(LIBK_F1))
    {
        helpScreen = !helpScreen;
        panelsMeshUpdate = true;
    }

    // Turns on/off sound
    if (engine->IsKeyPressed(LIBK_T))
    {
        bool soundEnabled = !cfg.GetBool("soundEnabled", true);

        engine->SetState(LIB_AUDIO_VOLUME, soundEnabled ? 1.0f : 0.0f);
        cfg.SetBool("soundEnabled", soundEnabled);
    }

    // Turns on/off the tetromino helper
    if (engine->IsKeyPressed(LIBK_H))
    {
        helperEnabled = !helperEnabled;
        cfg.SetBool("helperEnabled", helperEnabled);
        blocksMeshUpdate = true;
    }

    // Game logic
    if (!gameOver)
    {
        // Moves the tetromino down based on fall delay
        if (fallDelay.Seconds() >= tetrominoFallStartDelay / (1.0f + ((level.Get() - 1) * tetrominoFallDecreaseCoeff)))
        {
            fallDelay.Restart();
            MoveDown();
        }

        // Moves the tetromino left
        if ((engine->IsKeyPressed(LIBK_A) || engine->IsKeyPressed(LIBK_LEFT)) ||
            (engine->IsKey(LIBK_A) || engine->IsKey(LIBK_LEFT)) && leftTetrominoDelay.Seconds() >= keyDelay)
        {
            leftTetrominoDelay.Restart();
            MoveLeft();
        }

        // Moves the tetromino right
        if ((engine->IsKeyPressed(LIBK_D) || engine->IsKeyPressed(LIBK_RIGHT)) ||
            (engine->IsKey(LIBK_D) || engine->IsKey(LIBK_RIGHT)) && (rightTetrominoDelay.Seconds() >= keyDelay))
        {
            rightTetrominoDelay.Restart();
            MoveRight();
        }

        // Moves the tetromino down
        if ((engine->IsKeyPressed(LIBK_S) || engine->IsKeyPressed(LIBK_DOWN)) ||
            (engine->IsKey(LIBK_S) || engine->IsKey(LIBK_DOWN)) && downTetrominoDelay.Seconds() >= keyDelay)
        {
            fallDelay.Restart();
            downTetrominoDelay.Restart();
            MoveDown();
        }

        // Rotates the tetromino
        if (engine->IsKeyPressed(LIBK_W) || engine->IsKeyPressed(LIBK_UP))
        {
            Rotate();
        }

        // Drops the tetromino
        if (engine->IsKeyPressed(LIBK_SPACE))
        {
            fallDelay.Restart();
            Drop();
        }

        if (IsTetrominoDropped())
        {
            // Adds tetromino blocks in the field
            for (int i = 0; i < tetrominoWidth; i++)
            {
                for (int j = 0; j < tetrominoHeight; j++)
                {
                    if (!tetromino.Blocks()[i][j].filled)
                        continue;

                    field[tetromino.pos.x + i][tetromino.pos.y + j].filled = true;
                    field[tetromino.pos.x + i][tetromino.pos.y + j].color = tetromino.Blocks()[i][j].color;
                }
            }

            RemoveFilledLines();
            snd_drop->Play();

            tetromino.New();
            tetromino.pos.x = (fieldWidth / 2) - (tetrominoWidth / 2);

            if (IsTetrominoDropped() || !IsThereSpaceForNewTetromino())
            {
                gameOver = true;
                panelsMeshUpdate = true;
            }

            blocksMeshUpdate = true;
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
}

/*
===================
Game::LoadRecord
===================
*/
void Game::LoadRecord()
{
    libStr path;
    GetDataPath(path);
    path.Append("/Record.sav");

    libFile f(path.Get(), libFile::Mode::ReadBin);

    if (!f.IsOpened())
        return;

    f.Read(&record, sizeof(libHolder<int>), 1);
    f.Close();
}

/*
===================
Game::SaveRecord
===================
*/
void Game::SaveRecord()
{
    if (score.Get() <= record.Get())
        return;

    record.Set(score);

    libStr path;
    GetDataPath(path);
    path.Append("/Record.sav");

    libFile f(path.Get(), libFile::Mode::WriteBin);
    f.Write(&record, sizeof(libHolder<int>), 1);
    f.Close();
}

/*
===================
Game::GetDataPath
===================
*/
void Game::GetDataPath(libStr &path)
{
    path.Clear();
    libDir::GetLocalDataLocation(path);
    path.Append("/Tetris-2012");
    libDir::Create(path.Get());
}

/*
===================
Game::UpdateBlocksMesh
===================
*/
void Game::UpdateBlocksMesh()
{
    mesh_blocks->Clear();

    // The current tetromino
    if (IsThereSpaceForNewTetromino())
    {
        for (int i = 0; i < tetrominoWidth; i++)
        {
            for (int j = 0; j < tetrominoHeight; j++)
            {
                if (!tetromino.Blocks()[i][j].filled)
                    continue;

                float x = margin + blockSize * (i + tetromino.pos.x);
                float y = margin + blockSize * (j + tetromino.pos.y);
                AddBlockMesh(mesh_blocks, x, y, tetromino.Blocks()[i][j].color);
            }
        }
    }

    // The tetromino helper
    if (helperEnabled && !gameOver)
    {
        int tempX = tetromino.pos.x;
        int tempY = tetromino.pos.y;

        while (!IsTetrominoDropped())
            tetromino.pos.y++;

        // Show only if there is enough space
        if (tetromino.pos.y - tetrominoHeight > tempY)
        {
            for (int i = 0; i < tetrominoWidth; i++)
            {
                for (int j = 0; j < tetrominoHeight; j++)
                {
                    if (!tetromino.Blocks()[i][j].filled)
                        continue;

                    float x = margin + blockSize * (i + tetromino.pos.x);
                    float y = margin + blockSize * (j + tetromino.pos.y);
                    AddBlockMesh(mesh_blocks, x, y, tetromino.Blocks()[i][j].color, libColor(1.0f, 1.0f, 1.0f, 0.33f));
                }
            }
        }

        tetromino.pos.x = tempX;
        tetromino.pos.y = tempY;
    }

    // Blocks in the field
    for (int i = 0; i < fieldWidth; i++)
        for (int j = 0; j < fieldHeight; j++)
            if (field[i][j].filled)
                AddBlockMesh(mesh_blocks, margin + blockSize * i, margin + blockSize * j, field[i][j].color);

    for (int i = 0; i < tetrominoWidth; i++)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            if (!tetromino.NextTetromino()[j][i])
                continue;

            float x = libCast<float>(windowWidth - blockSize - blockSize * tetrominoWidth + blockSize * i);
            float y = margin + blockSize + blockSize * j;
            AddBlockMesh(mesh_blocks, x, y, libCast<Block::Color>(tetromino.NextID()));
        }
    }
}

/*
===================
Game::UpdatePanelsMesh
===================
*/
void Game::UpdatePanelsMesh()
{
    mesh_panel->Clear();
    mesh_border->Clear();

    // Help screen
    if (helpScreen)
    {
        // Help panel
        libQuad quad(libVertex(blockSize, blockSize),
                     libVertex(windowWidth - blockSize, windowHeight - blockSize));

        mesh_panel->Add(quad);
        AddBorderMesh(mesh_border, quad, LIB_COLOR_BLACK);
    }
    // Game screen
    else
    {
        // Game panel
        libQuad quad(libQuad(libVertex(margin, margin),
                             libVertex(margin + blockSize * fieldWidth, margin + blockSize * fieldHeight)));

        mesh_panel->Add(quad);
        AddBorderMesh(mesh_border, quad, LIB_COLOR_BLACK);

        // Game panel border
        AddBorderMesh(mesh_border, libQuad(libVertex(margin, margin),
                                           libVertex(margin + blockSize * fieldWidth, margin + blockSize * fieldHeight)), LIB_COLOR_BLACK);

        // Next tetromino panel
        mesh_panel->Add(libQuad(libVertex(windowWidth - blockSize - blockSize * tetrominoWidth, margin),
                                libVertex(windowWidth - blockSize, margin + blockSize + blockSize * tetrominoHeight)));

        AddBorderMesh(mesh_border, libQuad(libVertex(windowWidth - blockSize * 5, margin),
                                           libVertex(windowWidth - blockSize, margin + blockSize)), LIB_COLOR_BLACK);

        AddBorderMesh(mesh_border, libQuad(libVertex(windowWidth - blockSize * 5, margin + blockSize),
                                           libVertex(windowWidth - blockSize, margin + blockSize + blockSize * tetrominoHeight)), LIB_COLOR_BLACK);

        // Other panels
        for (int i = 0; i < 4; i++)
        {
            float offsetY = libCast<float>(blockSize * 3 * i);

            mesh_panel->Add(libQuad(libVertex(windowWidth - blockSize * 5, margin + blockSize * 6 + offsetY),
                                    libVertex(windowWidth - blockSize, margin + blockSize * 8 + offsetY)));

            AddBorderMesh(mesh_border, libQuad(libVertex(windowWidth - blockSize * 5, margin + blockSize * 6 + offsetY),
                                               libVertex(windowWidth - blockSize, margin + blockSize * 7 + offsetY)), LIB_COLOR_BLACK);

            AddBorderMesh(mesh_border, libQuad(libVertex(windowWidth - blockSize * 5, margin + blockSize * 6 + offsetY),
                                               libVertex(windowWidth - blockSize, margin + blockSize * 8 + offsetY)), LIB_COLOR_BLACK);
        }

        // Game over panel
        if (gameOver)
        {
            libQuad quad(libVertex(margin + blockSize, margin + blockSize * 5, 0.5f),
                         libVertex(margin + blockSize * 9, margin + blockSize * 7, 0.5f));

            mesh_panel->Add(quad);
            AddBorderMesh(mesh_border, quad, LIB_COLOR_BLACK);
        }
    }
}

/*
===================
Game::AddBlockMesh
===================
*/
void Game::AddBlockMesh(libMesh *buffer, float x, float y, Block::Color colorType, const libColor &color) const
{
    const float coords[7][4] = { {0.0f, 0.0f, blockSize, blockSize},
                               {blockSize, 0.0f, blockSize * 2.0f, blockSize},
                               {blockSize * 2.0f, 0.0f, blockSize * 3.0f, blockSize },
                               {0.0f, blockSize, blockSize, blockSize * 2.0f},
                               {blockSize, blockSize, blockSize * 2.0f, blockSize * 2.0f},
                               {blockSize * 2.0f, blockSize, blockSize * 3.0f, blockSize * 2.0f},
                               {0, blockSize * 2.0f, blockSize, blockSize * 3.0f} };

    int width = tex_blocks->Width();
    int height = tex_blocks->Height();

    libQuad quad(libVertex(0.0f, 0.0f, coords[colorType][0] / width, coords[colorType][1] / height),
                 libVertex(blockSize, blockSize, coords[colorType][2] / width, coords[colorType][3] / height));

    quad.SetColor(color);

    mesh_blocks->Add(quad, libVec3(x, y, 0.0f));
}

/*
===================
Game::AddBorderMesh
===================
*/
void Game::AddBorderMesh(libMesh *buffer, const libQuad &quad, const libColor &color) const
{
    engine->DrawLine(buffer, libVertex(quad.v[0].pos, color), libVertex(quad.v[1].pos, color), 2.0f);
    engine->DrawLine(buffer, libVertex(quad.v[1].pos, color), libVertex(quad.v[2].pos, color), 2.0f);
    engine->DrawLine(buffer, libVertex(quad.v[2].pos, color), libVertex(quad.v[3].pos, color), 2.0f);
    engine->DrawLine(buffer, libVertex(quad.v[3].pos, color), libVertex(quad.v[0].pos, color), 2.0f);
}

/*
===================
Game:NewGame
===================
*/
void Game::NewGame()
{
    for (int i = 0; i < fieldWidth; i++)
        for (int j = 0; j < fieldHeight; j++)
            field[i][j].filled = false;

    tetromino.New();
    tetromino.pos.x = (fieldWidth / 2) - (tetrominoWidth / 2);

    gameOver = false;
    score.Set(0);
    scoreGoal.Set(startupScoreGoal);
    level.Set(1);

    fallDelay.Restart();
    leftTetrominoDelay.Restart();
    rightTetrominoDelay.Restart();
    downTetrominoDelay.Restart();
    blocksMeshUpdate = true;
    panelsMeshUpdate = true;
}

/*
===================
Game::RemoveFilledLines
===================
*/
void Game::RemoveFilledLines()
{
    int numOfFilledLines = 0;

    for (int i = fieldHeight - 1; i > 0; i--)
    {
        bool filled = true;

        for (int j = 0; j < fieldWidth; j++)
            if (!field[j][i].filled)
                filled = false;

        if (filled)
        {
            for (int j = 0; j < fieldWidth; j++)
                field[j][i].filled = false;

            for (int len = i; len > 0; len--)
            {
                for (int j = 0; j < fieldWidth; j++)
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
            score.Set(score.Get() + scoreSingle * level.Get());
        else if (numOfFilledLines == 2)
            score.Set(score.Get() + scoreDouble * level.Get());
        else if (numOfFilledLines == 3)
            score.Set(score.Get() + scoreTriple * level.Get());
        else
            score.Set(score.Get() + scoreQuadruple * level.Get());

        if (score.Get() >= maxScore)
        {
            score.Set(maxScore);
            gameOver = true;
            panelsMeshUpdate = true;
        }
        else if (score.Get() >= scoreGoal.Get())
        {
            level.Set(level.Get() + 1);
            scoreGoal.Set(scoreGoal.Get() * scoreGoalMultiplier);

            if (scoreGoal.Get() > maxScore)
                scoreGoal.Set(maxScore);
        }

        blocksMeshUpdate = true;
    }
}

/*
===================
Game::IsTetrominoDropped
===================
*/
bool Game::IsTetrominoDropped() const
{
    for (int i = 0; i < tetrominoWidth; i++)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            if (!tetromino.Blocks()[i][j].filled)
                continue;

            if (tetromino.Blocks()[i][j + 1].filled && field[tetromino.pos.x + i][tetromino.pos.y + j + 1].filled)
                continue;

            if (tetromino.pos.y + j + 1 == fieldHeight || field[tetromino.pos.x + i][tetromino.pos.y + j + 1].filled)
                return true;
        }
    }

    return false;
}

/*
===================
Game::IsThereSpaceForNewTetromino
===================
*/
bool Game::IsThereSpaceForNewTetromino() const
{
    for (int i = 0; i < tetrominoWidth; i++)
        for (int j = 0; j < tetrominoHeight; j++)
            if (tetromino.Blocks()[i][j].filled && field[tetromino.pos.x + i][tetromino.pos.y + j].filled)
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
    for (int i = 0; i < tetrominoWidth; i++)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            if (tetromino.pos.x + i <= 0 && tetromino.Blocks()[i][j].filled)
                return;

            if (tetromino.Blocks()[i][j].filled && field[tetromino.pos.x + i - 1][tetromino.pos.y + j].filled)
                return;
        }
    }

    tetromino.pos.x--;
    blocksMeshUpdate = true;
}

/*
===================
Game::MoveRight
===================
*/
void Game::MoveRight()
{
    for (int i = tetrominoWidth - 1; i >= 0; i--)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            if (tetromino.pos.x + i >= fieldWidth - 1 && tetromino.Blocks()[i][j].filled)
                return;

            if (tetromino.Blocks()[i][j].filled && field[tetromino.pos.x + i + 1][tetromino.pos.y + j].filled)
                return;
        }
    }

    tetromino.pos.x++;
    blocksMeshUpdate = true;
}

/*
===================
Game::MoveDown
===================
*/
void Game::MoveDown()
{
    if (!IsTetrominoDropped())
        tetromino.pos.y++;

    blocksMeshUpdate = true;
}

/*
===================
Game::Drop
===================
*/
void Game::Drop()
{
    while (!IsTetrominoDropped())
        MoveDown();
}

/*
===================
Game::Rotate
===================
*/
void Game::Rotate()
{
    bool flag;
    bool allowRotate = true;
    int xOld = tetromino.pos.x;

    // Moves the tetromino to the right from the wall if there is not enough free space for rotations
    do
    {
        flag = true;

        for (int i = 0; i < -tetromino.pos.x; i++)
            for (int j = 0; j < tetrominoHeight; j++)
                if (tetromino.NextRotation()[j][i])
                    flag = false;
    
        if (!flag)
            MoveRight();

    } while (!flag);

    // Moves the tetromino to the left from the wall if there is not enough free space for rotations
    do
    {
        flag = true;

        for (int i = tetrominoWidth - 1; i > fieldWidth - 1 - tetromino.pos.x; i--)
            for (int j = 0; j < tetrominoHeight; j++)
                if (tetromino.NextRotation()[j][i])
                    flag = false;

        if (!flag)
            MoveLeft();

    } while (!flag);

    for (int i = 0; i < tetrominoWidth; i++)
    {
        for (int j = 0; j < tetrominoHeight; j++)
        {
            // Doesn't allow rotations of the tetromino when it's too low
            if (tetromino.NextRotation()[j][i] && tetromino.pos.y + j >= fieldHeight)
                allowRotate = false;

            // Doesn't allow rotations of the tetromino if there are any other blocks
            if (tetromino.NextRotation()[j][i] && !tetromino.Blocks()[j][i].filled)
                if (field[tetromino.pos.x + i][tetromino.pos.y + j].filled)
                    allowRotate = false;
        }
    }

    if (allowRotate)
        tetromino.Rotate();
    else
        tetromino.pos.x = xOld;

    blocksMeshUpdate = true;
}
