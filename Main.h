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

#include "../libEngine/Include/Engine.h"

#define TETRIS_VERSION "1.2.1"

#ifdef LIB_RELEASE
    #define ENGINE_PATH "./"
    #define DATA_PACK   "Data.lpk//"
#else
    #define ENGINE_PATH "../../libEngine/Binaries/"
    #define DATA_PACK "Data/"       
#endif

constexpr int windowWidth = 544;
constexpr int windowHeight = 704;
