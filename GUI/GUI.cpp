#include "gui.h"
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    WNDCLASSW wc = { 0 };
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"ParentWindowClass";
    wc.lpfnWndProc = windowProcedure;
    if (!RegisterClassW(&wc))
    {
        return -1;
    }
    parentHWND = CreateWindowW(L"ParentWindowClass", L"White Packets", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, height, width, NULL, NULL, NULL, NULL);
    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
LRESULT CALLBACK windowProcedure(HWND parentHWND, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            addMenus(parentHWND);
            addControls(parentHWND);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(parentHWND, msg, wp, lp);
    }

}

void addControls(HWND parentHWND)
{
    controlIDs controlIDS;
    lvPackets = new ListView(parentHWND, 0, 0, 600, 400, lvPacketID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvPackets->add_column(120, L"Caller");
    lvPackets->add_column(120, L"Type");
    lvPackets->add_column(120, L"header");
    lvPackets->add_column(1500, L"Data");
    lvPackets->showLV();

    sniffPackets = new Control(parentHWND, 0, 410, 50, 25, sniffPacketsID, L"Button", L"Start");
    clearLVPackets = new Control(parentHWND, 55, 410, 50, 25, clearLVPacketsID, L"Button", L"Clear");
    packetTextBox = new Control(parentHWND, 105, 405, 450, 35, packetTextBoxID, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL);
    lvFilters = new ListView(parentHWND, 600, 0, 175, 200,lvFiltersID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvFilters->add_column(100, L"Type");
    lvFilters->add_column(100, L"header");
    lvFilters->showLV();


}
void addMenus(HWND parenthWnd)
{
    hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, 1, L"Save");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Load");
    SetMenu(parenthWnd, hFileMenu);

}