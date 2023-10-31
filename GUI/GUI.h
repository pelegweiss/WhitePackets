#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <Windows.h>
#include "controls.h"
#include <thread>
#include <iostream>
#include "injector.h"
#include "guiFunctions.h"
#include <chrono>
#include <sstream>
#include "Json/json.hpp"
#include <fstream>

#include <strsafe.h>
#include "windowsx.h"
using jsonf = nlohmann::json;

const int height = 800;
const int width = 1200;
HMENU hFileMenu, hListViewPacketPopUpMenu, hListViewFiltersPopUpMenu,filterMenu;
HWND parentHWND,settingsHWND;
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK settingsProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK subClassLVPacketsData(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
int getListViewItemByCords(HWND listViewHwnd, POINT pt);
void enableToolTip(HWND hwndToolTip, TOOLINFO toolInfo);
void disableToolTip(HWND hwndToolTip, TOOLINFO toolInfo);
void SubclassListView(HWND hwndListView);
void addControls(HWND);
void addSettingsControl(HWND);
void addMenus(HWND);
Window* mainWindow;
Window* settingsWindow;
ListView* lvPackets;
ListView* lvFilters;
ListView* lvPacketsData;
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
Control* lvPacketsDataToolTip;
bool isSettingsOpen = false;
std::wstring maplestoryPath = L"";
std::wstring dllPath = L"";
bool sniff = false;
enum controlIDs
{
	lvPacketID, lvFiltersID, sniffPacketsID, clearLVPacketsID, packetTextBoxID, sendPacketID, recvPacketID, autoScrollID, filterTextBoxID, filterHeaderID, blockHeaderID, settingsButton, maplestoryPathTextBoxID, dllPathTextBoxID, maplestoryPathButtonID, dllPathButtonID, launchButtonID, removeFilterID, lvPacketsFilterID, lvPacketsBlockID, saveFiltersID, loadFiltersID, lvPacketsDataID, lvPacketsDataToolTipID
};

void pipeHandler();

void draw_text(HDC hdc, RECT& iR, COLORREF color, std::wstring Text);
std::wstring getTextFromBox(HWND boxHwnd, bool RemoveSpaces);
std::wstring open_file(HWND hWnd, const wchar_t* fileFilter);
std::wstring save_file(HWND hWnd);
bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv);
void showPopUpMenu(HMENU popUpMenu,HWND mainWindowHWND,LPARAM lowParam);
Packet processPacketFromTextBox(std::wstring data);
Packet processPacketFromWstring(std::wstring data);
std::vector<WORD> blockedHeaders;
BOOL isPipeToDLLConnected = false;
extern Pipe pipeToDLL;
