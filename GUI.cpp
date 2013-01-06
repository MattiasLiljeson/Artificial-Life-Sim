#include "GUI.h"

GUI::GUI(int mapSize, LPTSTR titleText)
{
	colors[0] = 0x0;
	colors[1] = FOREGROUND_GREEN;
	colors[2] = FOREGROUND_BLUE;
	colors[3] = FOREGROUND_RED;

	writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	readHandle = GetStdHandle(STD_INPUT_HANDLE);
	this->mapSize = mapSize;

	xCharSize = 4;
	yCharSize = 4;
	
	xSize = mapSize;
	ySize = mapSize + 1; // 1 row extra for text output

	bufferSize.X = xSize;
	bufferSize.Y = ySize;

	bufferCoord.X = 0;
	bufferCoord.Y = 0;

	bufferRegion.Top = 0;
	bufferRegion.Left = 0;
	bufferRegion.Right = xSize - 1;
	bufferRegion.Bottom = ySize - 1;

	turn = 0;

	screenBuffer = new CHAR_INFO[xSize*ySize];

	SetConsoleTitle(titleText);
}

GUI::~GUI()
{
	delete [] screenBuffer;
}

void GUI::init()
{
	int debug; // DEBUG

	// Get font struct, set font params and set font struct
	PCONSOLE_FONT_INFOEX font =  new CONSOLE_FONT_INFOEX();
	font->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if(!GetCurrentConsoleFontEx(writeHandle, false, font))
		showError(TEXT("GetCurrentConsoleFontEx()"));
	font->dwFontSize.X = xCharSize;
    font->dwFontSize.Y = yCharSize;
	if(!SetCurrentConsoleFontEx(writeHandle, false, font))
		showError(TEXT("SetCurrentConsoleFontEx()"));
	delete font;

	// Set buffer size
    if(!SetConsoleScreenBufferSize(writeHandle, bufferSize))
		showError(TEXT("SetConsoleScreenBufferSize()"));

	// Set window size tio the same size as the buffer
	if(!SetConsoleWindowInfo(writeHandle, true, &bufferRegion))
		showError(TEXT("SetConsoleWindowInfo()"));


	// Get and set screen buffer infoex
	PCONSOLE_SCREEN_BUFFER_INFOEX scrBufInfo = new CONSOLE_SCREEN_BUFFER_INFOEX;
	scrBufInfo->cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	if(!GetConsoleScreenBufferInfoEx(writeHandle, scrBufInfo))
		showError(TEXT("GetConsoleScreenBufferInfoEx()"));
	scrBufInfo->dwSize = bufferSize;
	scrBufInfo->dwCursorPosition.X = 0;
	scrBufInfo->dwCursorPosition.Y = ySize;
	scrBufInfo->dwMaximumWindowSize = bufferSize;
	if(!SetConsoleScreenBufferInfoEx(writeHandle, scrBufInfo))
		showError(TEXT("SetConsoleScreenBufferInfoEx()"));

	// Init buffer params and init buffer
	//if(!ReadConsoleOutput(writeHandle, screenBuffer, bufferSize, bufferCoord, &bufferRegion));
		//showError(TEXT("ReadConsoleOutput()"));
	//std::cin.get(); // DEBUG
}

void GUI::setChar(int i, int j, char symbol, unsigned short attributes)
{
	screenBuffer[i*mapSize + j].Char.AsciiChar = symbol;
	screenBuffer[i*mapSize + j].Attributes = (WORD)attributes;
}

void GUI::draw()
{
	if(!WriteConsoleOutput(writeHandle, screenBuffer, bufferSize, bufferCoord, &bufferRegion))
		showError("WriteConsoleOutput()");

	// Clear the buffer
	for(int i=0; i<xSize; i++)
	{
		for(int j=0; j<ySize; j++)
		{
			screenBuffer[i*ySize + j].Char.AsciiChar = '#';
			screenBuffer[i*ySize + j].Attributes = colors[3];
		}
	}

	turn++;
}

void GUI::showError()
{
	showError(TEXT("someFunction()"));
}

// Shameless copy-paste from msdn
void GUI::showError(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    //ExitProcess(dw); 
}