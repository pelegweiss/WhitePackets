#pragma once
#include <Windows.h>
#include "controls.h"
#include <thread>
#include <iostream>
#include "injector.h"
#include "guiFunctions.h"
#include <chrono>
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
const int height = 550;
const int width = 825;
HMENU hFileMenu, hListViewPacketPopUpMenu, hListViewFiltersPopUpMenu;
HWND parentHWND,settingsHWND;
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK settingsProcedure(HWND, UINT, WPARAM, LPARAM);
void addControls(HWND);
void addSettingsControl(HWND);
void addMenus(HWND);
Window* mainWindow;
Window* settingsWindow;
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
Control* maplestoryPathTextBox;
Control* dllPathTextBox;
Control* dllPathButton;
Control* maplestoryPathButton;
Control* launchButton;
Pipe pipeToDLL(L"pipeToDLL");
bool isSettingsOpen = false;
std::wstring maplestoryPath = L"";
std::wstring dllPath = L"";
bool sniff = false;
enum controlIDs
{
	lvPacketID,lvFiltersID, sniffPacketsID, clearLVPacketsID,packetTextBoxID,sendPacketID,recvPacketID,autoScrollID,filterTextBoxID,filterHeaderID,blockHeaderID,settingsButton,maplestoryPathTextBoxID,dllPathTextBoxID,maplestoryPathButtonID,dllPathButtonID,launchButtonID, removeFilterID,lvPacketsFilterID,lvPacketsBlockID
};

void pipeHandler();

wchar_t* getTextFromBox(HWND boxHwnd, bool RemoveSpaces);
std::wstring open_file(HWND hWnd);
bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv);

