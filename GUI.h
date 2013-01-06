#ifndef GUI_H
#define GUI_H

#include <Windows.h>
#include <strsafe.h> // Error msg
#include <iostream>
#include "Vec.h"

#define _WIN32_WINNT_WIN7 0x0601	// Tell VS which OS we're using
#define _WIN32_WINNT 0x0601			// --"--
#define NTDDI_WIN7 0x06010000		// --"--
#define WIN32_LEAN_AND_MEAN			// Skip unnecessary win32 stuff

class GUI
{
private:
	HANDLE writeHandle;
	HANDLE readHandle;

	short mapSize;
	short xSize;
	short ySize;
	short xCharSize;
	short yCharSize;

	int turn;

	CHAR_INFO* screenBuffer; //Used as 2d-array
	COORD bufferSize;
	COORD bufferCoord;
	SMALL_RECT bufferRegion;

public:
	unsigned short colors[4];

	GUI(int mapSize, LPTSTR titleText);
	~GUI();
	void init();
	void setChar(int i, int j, char symbol, unsigned short attributes);
	void draw();
	void showError();
	void showError(LPTSTR lpszFunction);
};

#endif //GUI_H