cmake_minimum_required (VERSION 2.8)

set (PROJECT Tetris-2012)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set (BUILD_TYPE LIB_RELEASE)
	set (DEBUG_POSTFIX)
else (CMAKE_BUILD_TYPE STREQUAL "Release")
	set (BUILD_TYPE LIB_DEBUG)
	set (DEBUG_POSTFIX _dbg)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

# Build name
set (BUILD_NAME Tetris${DEBUG_POSTFIX})

# Where libs library should be
if (WIN32)
	set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Win32/libs${DEBUG_POSTFIX})
else (WIN32)
	set (MOUNT_LIBS ${CMAKE_SOURCE_DIR}/../../libEngine/Lib/Unix/libs${DEBUG_POSTFIX})
endif (WIN32)

# Where the source code should be

set (MOUNT_DIR ..)

# Where we are building to
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${MOUNT_DIR}/Tetris-2012/)

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
	set (RESOURCES ${MOUNT_DIR}/Resources/Main.rc)
else (WIN32)
	set (RESOURCES)
endif (WIN32)

if (MSVC)
	add_definitions(/D${BUILD_TYPE} /W3)
else (MSVC)
	add_definitions(-D${BUILD_TYPE})
endif (MSVC)

add_definitions("-std=c++11")
add_executable (${BUILD_NAME} ${GUI_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

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
	target_link_libraries(${BUILD_NAME} ${MOUNT_LIBS}.a SDL2 dl)
endif (UNIX)
