#pragma once
#include <Windows.h>
#include "controls.h"
#include <thread>
#include <iostream>
#include "pipe.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
const int height = 550;
const int width = 825;
HMENU hFileMenu;
HWND parentHWND;
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void addControls(HWND);
void addMenus(HWND);
ListView* lvPackets;
ListView* lvFilters;
Control* sniffPackets;
Control* clearLVPackets;
Control* packetTextBox;
Control* sendPacket;
Control* recvPacket;
Control* autoScroll;
Control* filterTextBox;
Control* filterHeader;
Control* blockHeader;
Pipe pipeToGui(L"pipeToGui");
Pipe pipeToDLL(L"pipeToDLL");
enum controlIDs
{
	lvPacketID,lvFiltersID, sniffPacketsID, clearLVPacketsID,packetTextBoxID,sendPacketID,recvPacketID,autoScrollID,filterTextBoxID,filterHeaderID,blockHeaderID
};

void pipeHandler();

wchar_t* getTextFromBox(HWND boxHwnd, bool RemoveSpaces);
void messagesHandler(pipeMessage message);
