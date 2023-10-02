#include "controls.h"
HWND Control::Get_Hwnd()
{
	return this->hWnd;
}
HMENU Control::get_ControlID()
{
	return this->ControlID;
}
Control::Control() { this->hWnd = NULL; this->ControlID = NULL; };
Control::Control(HWND hwndParent, int x1, int y1, int x2, int y2, int id, LPCWSTR type, LPCWSTR Text)
{
	this->hWnd = CreateWindowExW(0, type, Text, WS_VISIBLE | WS_CHILD, x1, y1,
		x2,
		y2,
		hwndParent,
		(HMENU)id,
		NULL,
		NULL
	);
	this->ControlID = (HMENU)id;
}
Control::Control(HWND hwndParent, int x1, int y1, int x2, int y2, int id, LPCWSTR type, LPCWSTR Text, DWORD styles)
{
	this->hWnd = CreateWindowExW(0, type, Text, styles, x1, y1,
		x2,
		y2,
		hwndParent,
		(HMENU)id,
		NULL,
		NULL
	);
	this->ControlID = (HMENU)id;
}

ListView::ListView(HWND hwndParent, int x1, int y1, int x2, int y2, int id, DWORD styles, DWORD extendedStyles)
{
	this->hWnd = CreateWindowExW(0, WC_LISTVIEWW, NULL, styles, x1, y1,
		x2,
		y2,
		hwndParent,
		(HMENU)id,
		NULL,
		NULL
	);
	if (extendedStyles != NULL)
	{
		ListView_SetExtendedListViewStyle(this->hWnd, styles);
	}

	this->ControlID = (HMENU)id;

};
void ListView::showLV()
{
	ShowWindow(this->hWnd, 1);
}
void ListView::setExtendedStyltes(DWORD styles)
{
	ListView_SetExtendedListViewStyle(this->hWnd, styles);
}
void ListView::add_column(int width, std::wstring text)
{
	int column = Header_GetItemCount(ListView_GetHeader(this->hWnd));
	LVCOLUMNW c = {};
	c.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	c.cx = width;
	c.pszText = decltype(c.pszText)(text.c_str());
	ListView_InsertColumn(this->hWnd, column, &c);
}
void ListView::add_item(std::vector<std::wstring> data)
{
	m_v.emplace_back(data);
	ListView_SetItemCountEx(this->hWnd, m_v.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
	++this->m_itemcount;
	if (this->m_scroll) { ListView_EnsureVisible(this->hWnd, this->m_itemcount - 1, false); }
}
void ListView::clear_items()
{
	this->m_v.clear();
	this->m_itemcount = 0;
	ListView_SetItemCountEx(this->hWnd, this->m_itemcount, NULL);

}