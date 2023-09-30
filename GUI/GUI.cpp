#include "gui.h"
std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    FILE* pFile = nullptr;


    //Include these after process attach switch has been triggered.
    AllocConsole();
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    //std::thread thread_obj(pipeHandler);
    //pipeToDLL.createPipe();
    //pipeToDLL.waitForClient();
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

    WNDCLASSW settingsWC = { 0 };
    settingsWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
    settingsWC.hCursor = LoadCursor(NULL, IDC_ARROW);
    settingsWC.hCursor = LoadCursor(NULL, IDC_ARROW);
    settingsWC.hInstance = GetModuleHandleA(NULL);
    settingsWC.lpszClassName = L"settingsWindowClass";
    settingsWC.lpfnWndProc = settingsProcedure;
    if (!RegisterClassW(&settingsWC))
    {
        return -1;
    }

    parentHWND = CreateWindowW(L"ParentWindowClass", L"White Packets", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, width, height, NULL, NULL, NULL, NULL);

    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
LRESULT CALLBACK settingsProcedure(HWND settingsHWND, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_COMMAND:
        {
            switch (wp)
            {
                case maplestoryPathButtonID:
                {
                    std::wstring bufferPath = open_file(settingsHWND);
                    SetWindowText(maplestoryPathTextBox->Get_Hwnd(), bufferPath.c_str());
                    maplestoryPath = bufferPath;
                }
                break;
                case dllPathButtonID:
                {
                    std::wstring bufferPath = open_file(settingsHWND);
                    SetWindowText(dllPathTextBox->Get_Hwnd(), bufferPath.c_str());
                    maplestoryPath = dllPath;
                }
                break;
            }
            break;
        }
        break;
        case WM_CREATE:
        {
            addSettingsControl(settingsHWND);
        }
        break;

        case WM_CLOSE:
        {
            DestroyWindow(settingsHWND);
            isSettingsOpen = !isSettingsOpen;
        }
        break;

        default:
            return DefWindowProc(settingsHWND, msg, wp, lp);
    }
}
LRESULT CALLBACK windowProcedure(HWND parentHWND, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_NOTIFY:
        {
            switch (((NMHDR*)lp)->code)
            {
            case LVN_GETDISPINFO:
            {

                NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lp;
                int row = plvdi->item.iItem;
                int col = plvdi->item.iSubItem;
                if (plvdi->item.mask & LVIF_TEXT)
                {
                    {
                        if (wp == (WPARAM)lvPackets->get_ControlID())
                        {
                            plvdi->item.pszText = (LPWSTR)lvPackets->m_v[row][col].c_str();
                        }
                        else if (wp == (WPARAM)lvFilters->get_ControlID())
                        {
                            plvdi->item.pszText = (LPWSTR)lvFilters->m_v[row][col].c_str();
                        }
                    }
                }
                break;
            }
            case NM_CUSTOMDRAW: {
                LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lp;
                switch (lplvcd->nmcd.dwDrawStage) {
                case CDDS_PREPAINT:
                    return CDRF_NOTIFYITEMDRAW;
                    break;
                case CDDS_ITEMPREPAINT:
                    if (((int)lplvcd->nmcd.dwItemSpec % 2) == 0) {
                        lplvcd->clrText = RGB(0, 0, 0);
                        lplvcd->clrTextBk = RGB(245, 245, 245);
                    }
                    else {
                        lplvcd->clrText = RGB(0, 0, 0);
                        lplvcd->clrTextBk = RGB(255, 255, 255);
                    }
                    return CDRF_NEWFONT;
                    break;
                    //There would be some bits here for subitem drawing but they don't seem neccesary as you seem to want a full row color only
                case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                    return CDRF_NEWFONT;
                    break;
                }
                return TRUE;

            }

            }
        }
        break;
        case WM_COMMAND:
        {
            switch (wp)
            {
                case sendPacketID:
                {
                    pipeMessage message;
                    //message.type = L"Packet";
                    message.data = getTextFromBox(packetTextBox->Get_Hwnd(), true);
                    pipeToDLL.sendMessage(message);
                }
                break;
                case launchButtonID:
                {
                    ShellExecute(nullptr, L"open", maplestoryPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                    int i = 0;
                    HWND maplestoryHWND = FindWindowW(NULL, L"MapleStory");
                    while (maplestoryHWND == NULL && i < 10)
                    {
                        Sleep(1000);
                        i++;
                        maplestoryHWND = FindWindowW(NULL, L"MapleStory");

                    }
                    if (i == 9)
                    {
                        std::cout << "Failed to launch" << std::endl;
                        break;
                    }
                    std::cout << "Maplestory launched sucssesfully" << std::endl;
                }
                break;
                case settingsButton:
                {
                    if (isSettingsOpen == false)
                    {
                        settingsHWND = CreateWindowW(L"settingsWindowClass", L"Settings", WS_OVERLAPPEDWINDOW | WS_VISIBLE , 150, 150, 425, 120, parentHWND, NULL, NULL, NULL);
                        isSettingsOpen = !isSettingsOpen;
                    }

                }
                break;
            }
        }
        break;


        case WM_CREATE:
        {
            addMenus(parentHWND);
            addControls(parentHWND);
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
        default:
            return DefWindowProc(parentHWND, msg, wp, lp);
    }

}

void addControls(HWND parentHWND)
{
    controlIDs controlIDS;
    lvPackets = new ListView(parentHWND, 0, 0, 600, 400, lvPacketID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvPackets->add_column(160, L"Caller");
    lvPackets->add_column(160, L"Type");
    lvPackets->add_column(160, L"header");
    lvPackets->add_column(1500, L"Data");
    lvPackets->showLV();

    packetTextBox = new Control(parentHWND, 0, 405, 500, 35, packetTextBoxID, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL);

    sendPacket = new Control(parentHWND, 500, 405, 50, 35, sendPacketID, L"button", L"Send");
    recvPacket = new Control(parentHWND, 550, 405, 50, 35, recvPacketID, L"button", L"Recv");

    sniffPackets = new Control(parentHWND, 0, 450, 50, 25, sniffPacketsID, L"Button", L"Start");
    clearLVPackets = new Control(parentHWND, 55, 450, 50, 25, clearLVPacketsID, L"Button", L"Clear");

    CreateWindow(L"static", L"Auto scroll is: ", WS_VISIBLE | WS_CHILD, 110, 455, 90, 25, parentHWND, NULL, NULL, NULL);
    autoScroll = new Control(parentHWND, 205,450, 25, 25, autoScrollID, L"Button", L"ON");

    lvFilters = new ListView(parentHWND, 600, 0, 204, 200,lvFiltersID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvFilters->add_column(102, L"Type");
    lvFilters->add_column(102, L"header");
    lvFilters->showLV();
    CreateWindow(L"static", L"Header:", WS_VISIBLE | WS_CHILD, 600, 205, 50, 25, parentHWND, NULL, NULL, NULL);
    filterTextBox = new Control(parentHWND, 655, 205, 50, 20, filterTextBoxID, L"edit", L"");
    filterHeader = new Control(parentHWND, 705, 205, 50, 20, filterHeaderID,L"button", L"Filter");
    blockHeader = new Control(parentHWND, 755, 205, 50, 20, filterHeaderID, L"button", L"Block");
    launchButton = new Control(parentHWND, 700, 440, 100, 50, launchButtonID, L"button", L"Launch");
}
void addSettingsControl(HWND hwnd)
{
    CreateWindow(L"static", L"Maplestory Path:", WS_VISIBLE | WS_CHILD, 15, 15, 110, 25, hwnd, NULL, NULL, NULL);
    CreateWindow(L"static", L"DLL Path:", WS_VISIBLE | WS_CHILD, 60, 45, 100, 25, hwnd, NULL, NULL, NULL);
    maplestoryPathTextBox = new Control(hwnd, 130, 10, 250, 25, maplestoryPathTextBoxID, L"edit", maplestoryPath.c_str(), ES_READONLY | WS_BORDER | WS_CHILD | WS_VISIBLE);
    dllPathTextBox = new Control(hwnd, 130, 40, 250, 25, dllPathTextBoxID, L"edit", dllPath.c_str(), ES_READONLY | WS_BORDER | WS_CHILD | WS_VISIBLE);
    maplestoryPathButton = new Control(hwnd, 380, 10, 25, 25, maplestoryPathButtonID, L"Button", L"...");
    dllPathButton = new Control(hwnd, 380, 40, 25, 25, dllPathButtonID, L"Button", L"...");

}
void addMenus(HWND parenthWnd)
{
    hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, 1, L"Save");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Load");
    AppendMenu(hFileMenu, MF_STRING, settingsButton, L"Settings");
    SetMenu(parenthWnd, hFileMenu);

}



void pipeHandler()
{
    using namespace std::literals::chrono_literals;
    
    while (!pipeToGui.connectPipe())
    {
        std::cout << "Failed to connect pipe, retrying" << std::endl;
        std::this_thread::sleep_for(1s);
    }
    pipeMessage message = pipeToGui.readMessage();
    while (message.id != 1)
    {
        std::wcout << "Message ID: " << message.id << " Message Data" << " \42" << message.data  << "\42" << std::endl;
        messagesHandler(message);
        message = pipeToGui.readMessage();

    }
    std::cout << "Connection ended, pipe is no longer exist" << std::endl;
}

wchar_t* getTextFromBox(HWND boxHwnd, bool RemoveSpaces)
{
    wchar_t* packet_text = new wchar_t;
    int len = GetWindowTextLengthA(boxHwnd);
    GetWindowText(boxHwnd, packet_text, len + 1);
    int count = 0;
    //removing spaces
    if (RemoveSpaces)
    {
        for (int i = 0; packet_text[i]; i++)
        {
            if (packet_text[i] != ' ')
            {
                packet_text[count++] = packet_text[i];
            }
        }
        packet_text[count] = '\0';
    }
    return packet_text;
}


void messagesHandler(pipeMessage message)
{
    switch (message.id)
    {
        case 0:
        {
            
        }
        break;
        case 1:
        {

        }
        break;
        case 2:
        {
        }
        break;
        case 3:
        {

        }
        break;
   }
}

std::wstring open_file(HWND hWnd)
{
    OPENFILENAME ofn = { 0 };
    wchar_t szFile[MAX_PATH] = { 0 };

    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;

    // Create a filter string based on the fileType parameter
     ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return std::wstring(ofn.lpstrFile);
    }
    return L"";
}