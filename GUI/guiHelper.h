#pragma once
#include <vector>
#include <string>
#include "controls.h"
#include "Pipe.h"
//ListView functions
bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv);
void SubclassListView(HWND hwndListView, WNDPROC& g_pfnOrigListViewProc, LONG_PTR subClassLVPacketsData);
int getListViewItemByCords(HWND listViewHwnd, POINT pt);
void draw_text(HDC hdc, RECT& iR, COLORREF color, std::wstring Text);
void RemoveFilterItem(std::vector<std::vector<std::wstring>>& m_v, HWND hwnd, int index);
void ProcessBlockTypeAndSendMessage(bool isPipeToDLLConnected, const std::wstring& type, const std::wstring& header, Pipe& pipeToDLL);
void ProcessBlockFilterHeaderCase(const std::wstring& header, HWND parentHWND, ListView* lvFilters, bool isPipeToDLLConnected, Pipe& pipeToDLL, const std::wstring& caseType, std::vector<WORD>& blockedHeaders);
void lvFilterBlock(ListView* lvPackets, ListView* lvFilters, const std::wstring& caseType, std::vector<WORD>& blockedHeaders, bool isPipeToDLLConnected, Pipe& pipeToDLL);

//Wstring to Packet functions
void stringSegmentToWstring(std::wstring& segmentString, Packet& p, std::wistringstream& stream);
void bufferSegmentToWstring(std::wstring& segmentString, Packet& p);
void dataSegmentToWstring(std::wstring& segmentString, Packet& p);
Packet WstringToPacket(std::wstring data);

//Gui functions
void showPopUpMenu(HMENU popUpMenu, HWND mainWindowHWND, LPARAM lowParam);
POINT getMouseCordsByLP(LPARAM lp);

//File Dialogs functions
std::wstring save_file(HWND hWnd);
std::wstring open_file(HWND hWnd, const wchar_t* fileFilter);

//Text boxes functions
std::wstring getTextFromBox(HWND boxHwnd, bool RemoveSpaces);

//ToolTipFunctions
void enableToolTip(HWND hwndToolTip, TOOLINFO toolInfo);
void disableToolTip(HWND hwndToolTip, TOOLINFO toolInfo);

//bytesSegment to wstringSegment functions
std::vector<std::wstring> dataSegToWstringSeg(const std::wstring& type, std::vector<BYTE>& bytes);
std::vector<std::wstring> bufferSegToWstringSeg(const std::wstring& type, const std::vector<BYTE>& bytes);
std::vector<std::wstring> stringSegToWstringSeg(std::vector<BYTE>& bytes);
std::vector<std::wstring> segToWstring(Segment& segment);