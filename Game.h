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

#pragma once

#include "Tetromino.h"

constexpr int fieldWidth = 10;
constexpr int fieldHeight = 20;

/*
===============================================================================

    Game

===============================================================================
*/
class Game
{
public:

                        Game();

    bool                Init();
    void                Draw();
    void                Update();
    void                Free();

private:

    void                LoadRecord();
    void                SaveRecord();
    void                GetDataPath(libStr &path);
    
    void                UpdateBlocksMesh();
    void                UpdatePanelsMesh();
    void                AddBlockMesh(libMesh *buffer, float x, float y, Block::Color colorType, const libColor &color = libColor()) const;
    void                AddBorderMesh(libMesh *buffer, const libQuad &quad, const libColor &color) const;

    void                NewGame();
    void                RemoveFilledLines();
    bool                IsTetrominoDropped() const;
    bool                IsThereSpaceForNewTetromino() const;

    void                MoveLeft();
    void                MoveRight();
    void                MoveDown();
    void                Drop();
    void                Rotate();

    libPtr<libFont>     font;
    libPtr<libTexture>  tex_background;
    libPtr<libTexture>  tex_blocks;
    libPtr<libSound>    snd_drop;

    libPtr<libMesh>     mesh_background;
    libPtr<libMesh>     mesh_blocks;
    libPtr<libMesh>     mesh_panel;
    libPtr<libMesh>     mesh_border;
    bool                blocksMeshUpdate = false;
    bool                panelsMeshUpdate = false;

    Block               field[fieldWidth][fieldHeight];
    Tetromino           tetromino;

    bool                gameOver;
    bool                helpScreen;
    bool                helperEnabled;
    libHolder<int>      score;
    libHolder<int>      scoreGoal;
    libHolder<int>      level;
    libHolder<int>      record;

    float               keyDelay;
    libTimer            fallDelay;
    libTimer            leftTetrominoDelay;
    libTimer            rightTetrominoDelay;
    libTimer            downTetrominoDelay;

    libCfg              cfg;
};
