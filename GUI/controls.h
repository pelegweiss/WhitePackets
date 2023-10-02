#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <CommCtrl.h>
#include <iostream>

class Window
{
protected:
	std::wstring windowClass, windowTitle;
	HWND hWND;
	bool isWindowCreated;
public:
	HWND getHWND();
	Window();
	Window(std::wstring className, WNDPROC procedure, HINSTANCE hInst);
	bool createWindow(std::wstring title, int x1, int y1, int x2, int y2);
	bool isCreated();
	void setState(bool state);
};

class Control
{
protected:
	HWND hWnd;
	HMENU ControlID;
public:
	HWND Get_Hwnd();
	HMENU get_ControlID();
	Control();
	Control(HWND hwndParent, int x1, int y1, int x2, int y2, int id, LPCWSTR type, LPCWSTR Text);
	Control(HWND hwndParent, int x1, int y1, int x2, int y2, int id, LPCWSTR type, LPCWSTR Text, DWORD styles);
};
class ListView : public Control
{

public:
	std::vector<std::vector<std::wstring>> m_v;
	bool m_scroll{ true };
	int m_itemcount = 0;
	ListView(HWND hwndParent, int x1, int y1, int x2, int y2, int id, DWORD styles, DWORD extendedStyles);
	void showLV();
	void setExtendedStyltes(DWORD styles);
	void add_column(int width, std::wstring text);
	void add_item(std::vector<std::wstring> data);
	void clear_items();
};