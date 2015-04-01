# CMakeLists.txt
#
# Project: Tetris
#
# Currently build for the following:
#	Win32
#	Linux i386
#
# CMake required

cmake_minimum_required (VERSION 2.8)

set (PROJECT Tetris)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set (BUILD_NAME Tetris)
	set (BUILD_TYPE RELEASE)
else (CMAKE_BUILD_TYPE STREQUAL "Release")
	set (BUILD_NAME Tetris_dbg)
	set (BUILD_TYPE DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

# Where libs library should be
if (WIN32)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Win32/libs)
	else (CMAKE_BUILD_TYPE STREQUAL "Release")
		set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Win32/libs_dbg)
	endif (CMAKE_BUILD_TYPE STREQUAL "Release")
else (WIN32)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Unix/libs)
	else (CMAKE_BUILD_TYPE STREQUAL "Release")
		set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Unix/libs_dbg)
	endif (CMAKE_BUILD_TYPE STREQUAL "Release")
endif (WIN32)

# where the source code should be
set (MOUNT_DIR ..)

# where we are building to
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${MOUNT_DIR}/Tetris)

# Win32
if (WIN32)
	set (GUI_TYPE WIN32)
endif (WIN32)

# Unix
if (UNIX)
endif (UNIX)

# MacOS
IF (APPLE)
	SET (GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

project (${PROJECT})

set (SOURCES
	${MOUNT_DIR}/Figure.cpp
	${MOUNT_DIR}/Glass.cpp
	${MOUNT_DIR}/Main.cpp
	)

set (HEADERS
	${MOUNT_DIR}/Figure.h
	${MOUNT_DIR}/Glass.h
	${MOUNT_DIR}/Main.h
	)

if (WIN32)
	set (RESOURCES
		${MOUNT_DIR}/Resources/Main.rc
		)
else (WIN32)
	set (RESOURCES
		)
endif (WIN32)

if (MSVC)
	add_definitions(/D${BUILD_TYPE} /W3)
else (MSVC)
	add_definitions(-D${BUILD_TYPE})
endif (MSVC)

add_definitions("-std=c++11")
add_executable (${BUILD_NAME} ${GUI_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

#==========================================================
#	Linking libraries for Load/Unload dll libraries
#==========================================================

# Win32
if (WIN32)
	if (MSVC)
		target_link_libraries(${BUILD_NAME} ${MOUNT_LIBS}.lib user32.lib)
	else (MSVC)
		target_link_libraries(${BUILD_NAME} ${MOUNT_LIBS}.a user32.a)
	endif (MSVC)
endif (WIN32)

# Unix
if (UNIX)
	target_link_libraries(${BUILD_NAME} ${MOUNT_LIBS}.a dl)
endif (UNIX)
