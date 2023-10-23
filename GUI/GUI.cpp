#include "gui.h"
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    FILE* pFile = nullptr;


    //Include these after process attach switch has been triggered.
    AllocConsole();
    freopen_s(&pFile, "CONOUT$", "w", stdout);

    mainWindow = new Window(L"mainWndClass", windowProcedure,hInst);
    settingsWindow = new Window(L"settingsWndClass", settingsProcedure, hInst);
    mainWindow->createWindow(L"White Packets",0,0,width,height);
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
                    dllPath = bufferPath;
                }
                break;
            }
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
            settingsWindow->setState(!(settingsWindow->isCreated()));
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
                            switch (wp)
                            {
                            case lvPacketID:plvdi->item.pszText = (LPWSTR)lvPackets->m_v[row][col].c_str(); break;
                            case lvFiltersID:plvdi->item.pszText = (LPWSTR)lvFilters->m_v[row][col].c_str(); break;
                            }
                        }
                    }
                }
                break;
                case NM_CUSTOMDRAW: 
                {
                    LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lp;
                    switch (lplvcd->nmcd.dwDrawStage)
                    {
                        case CDDS_PREPAINT:
                            return CDRF_NOTIFYITEMDRAW;
                            break;
                        case CDDS_ITEMPREPAINT:
                        {
                            int isEven = (int)lplvcd->nmcd.dwItemSpec % 2;
                            switch (isEven)
                            {
                            case 0:
                            {
                                lplvcd->clrText = RGB(0, 0, 0);
                                lplvcd->clrTextBk = RGB(225, 225, 225);
                            }
                            break;
                            default:
                            {
                                lplvcd->clrText = RGB(0, 0, 0);
                                lplvcd->clrTextBk = RGB(255, 255, 255);
                            }
                            break;
                            }
                            return CDRF_NEWFONT;
                        }
                        break;
                        case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                            return CDRF_NEWFONT;
                    }
                    return TRUE;

                }
                break;
                case NM_CLICK:
                {
                    switch (wp)
                    {
                        case lvPacketID:
                        {
                            POINT cursor; // Getting the cursor position
                            GetCursorPos(&cursor);

                            NMITEMACTIVATE* nmia = (NMITEMACTIVATE*)lp;
                            if (nmia->iItem != -1)
                            {
                                int iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), -1, LVNI_SELECTED);
                                if(iPos != -1) {
                                    // iPos is the index of a selected item
                                    // do whatever you want with it
                                    std::wstring header = lvPackets->m_v[iPos][2];
                                    std::wstring data = lvPackets->m_v[iPos][3];
                                    std::wstring finalString = header + L" " + data;
                                    SetWindowText(packetTextBox->Get_Hwnd(), (LPWSTR)finalString.c_str());
                                }
                            }
                        }
                    }
                }
                break;
                case NM_RCLICK:
                {
                    switch (wp)
                    {
                        case lvPacketID:
                        {
                            showPopUpMenu(hListViewPacketPopUpMenu,parentHWND,lp);
                        }
                        break;
                        case lvFiltersID:
                        {
                            showPopUpMenu(hListViewFiltersPopUpMenu, parentHWND, lp);
                        }
                        break;
                    }
                }
                break;

            }
            
        }
        break;
        case WM_COMMAND:
        {
            switch (wp)
            {
                case launchButtonID:
                {
                    const int maxRetries = 3;
                    int retriesCounter = 0;
                    bool launched = false;
                    while (retriesCounter < maxRetries)
                    {
                        if (runMaplestory(maplestoryPath, dllPath) == false)
                        {
                            retriesCounter++;
                        }
                        else
                        {
                            std::cout << "Managed to launch and inject" << std::endl;
                            retriesCounter = 3;
                            launched = true;
                        }
                    }
                    if (launched == false)
                    {
                        std::cout << "Failed to launch and injcet" << std::endl;
                    }

                }
                break;
                case settingsButton:
                {
                    if (settingsWindow->isCreated() == false)
                    {
                        settingsWindow->createWindow(L"SettingsWindowClass", 150, 150, 425, 120);
                    }

                }
                break;
                case autoScrollID:
                {
                    lvPackets->m_scroll = !lvPackets->m_scroll;
                    if (lvPackets->m_scroll) { SendMessage(autoScroll->Get_Hwnd(), WM_SETTEXT, 0, (LPARAM)L"ON"); }
                    else { SendMessage(autoScroll->Get_Hwnd(), WM_SETTEXT, 0, (LPARAM)L"OFF"); }
                    break;
                }
                break;
                case clearLVPacketsID:
                {
                    lvPackets->clear_items();
                }
                break;
                case sniffPacketsID:
                {
                    sniff = !sniff;
                    if (sniff)
                    {
                        SendMessage(sniffPackets->Get_Hwnd(), WM_SETTEXT, 0, (LPARAM)L"Pause");
                    }
                    else
                    {
                        SendMessage(sniffPackets->Get_Hwnd(), WM_SETTEXT, 0, (LPARAM)L"Start");
                    }
                }
                break;
                case lvPacketsFilterID:
                {
                    //Get Header of selected item from list view
                    int iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), -1, LVNI_SELECTED);
                    //iterating through all selected items
                    while (iPos != -1)
                    {
                        // iPos is the index of a selected item

                        //getHeader based on iPos(index)
                        std::wstring header = lvPackets->m_v[iPos][2];
                        //Check if header is already blocked
                        std::vector<std::wstring> buf;
                        buf.emplace_back((std::wstring)L"Filter");
                        buf.emplace_back(header);
                        bool isFiltered = isHeaderFiltered(buf, lvFilters);
                        if (!isFiltered)
                        {
                            std::vector<std::wstring> v;
                            v.emplace_back((std::wstring)L"Filter");
                            v.emplace_back(header);
                            lvFilters->add_item(v);
                        }
                        //Move to next item
                        iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), iPos, LVNI_SELECTED);
                    }

                }
                break;

                case lvPacketsBlockID:
                {
                    //Get Header of selected item from list view
                    int iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), -1, LVNI_SELECTED);

                    //iterating through all selected items
                    while (iPos != -1)
                    {
                        // iPos is the index of a selected item
                        //getHeader based on iPos(index)
                        std::wstring header = lvPackets->m_v[iPos][2];
                        //Check if header is already blocked
                        std::vector<std::wstring> buf;
                        buf.emplace_back((std::wstring)L"Block");
                        buf.emplace_back(header);
                        bool isBlocked = isHeaderFiltered(buf, lvFilters);
                        if (!isBlocked)
                        {
                            std::vector<std::wstring> v;
                            v.emplace_back((std::wstring)L"Block");
                            v.emplace_back(header);
                            lvFilters->add_item(v);

                            unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
                            WORD wordValue = static_cast<WORD>(ulValue);
                            if (isPipeToDLLConnected == false)
                            {
                                blockedHeaders.emplace_back(wordValue);
                            }
                            else
                            {
                                pipeMessage message;
                                Header h;
                                h.action = 1;
                                h.header = wordValue;
                                message.id = bHeader;
                                message.data = (void*)&h;
                                pipeToDLL.sendBlockHeaderMessage(message);
                            }
                        }
                        iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), iPos, LVNI_SELECTED);


                    }
                    //Move to next Item
                    iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), iPos, LVNI_SELECTED);
                }
                break;
                case blockHeaderID:
                {
                    //Get header
                    std::wstring header = getTextFromBox(filterTextBox->Get_Hwnd(), false);
                    int len = GetWindowTextLength(filterTextBox->Get_Hwnd());
                    //Check for spaces
                    bool containSpace = false;
                    for (int i = 0; i < len; i++)
                    {
                        wchar_t buffer = header[i];
                        if (buffer == 32)
                        {
                            containSpace = true;
                            break;
                        }
                    }

                    //Check if header is too long
                    if (len > 4)
                    {
                        MessageBox(parentHWND, L"Your header contatin more than 2 bytes, please insert proper header", L"Error", MB_OK | MB_ICONSTOP);
                    }
                    else if (len < 4)
                    {
                        MessageBox(parentHWND, L"Your header contatin less than 2 bytes, please insert proper header", L"Error", MB_OK | MB_ICONSTOP);

                    }
                    else
                    {
                        std::vector<std::wstring> buf;
                        buf.emplace_back((std::wstring)L"Block");
                        buf.emplace_back((std::wstring)header);
                        bool isFiltered = isHeaderFiltered(buf, lvFilters);
                        if (!isFiltered)
                        {
                            lvFilters->add_item(buf);

                            unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
                            WORD wordValue = static_cast<WORD>(ulValue);
                            if (isPipeToDLLConnected == false)
                            {
                                blockedHeaders.emplace_back(wordValue);
                            }
                            else
                            {
                                pipeMessage message;
                                Header h;
                                h.action = 1;
                                h.header = wordValue;
                                message.id = bHeader;
                                message.data = (void*)&h;
                                pipeToDLL.sendBlockHeaderMessage(message);
                            }

                        }
                    }

                }
                break;
                case filterHeaderID:
                {
                    //Get header
                    std::wstring header = getTextFromBox(filterTextBox->Get_Hwnd(), false);
                    int len = GetWindowTextLength(filterTextBox->Get_Hwnd());
                    //Check for spaces
                    bool containSpace = false;
                    for (int i = 0; i < len; i++)
                    {
                        wchar_t buffer = header[i];
                        if (buffer == 32)
                        {
                            containSpace = true;
                            break;
                        }
                    }

                    //Check if header is too long
                    if (len > 4)
                    {
                        MessageBox(parentHWND, L"Your header contatin more than 2 bytes, please insert proper header", L"Error", MB_OK | MB_ICONSTOP);
                    }
                    else if (len < 4)
                    {
                        MessageBox(parentHWND, L"Your header contatin less than 2 bytes, please insert proper header", L"Error", MB_OK | MB_ICONSTOP);

                    }
                    else
                    {
                        std::vector<std::wstring> buf;
                        buf.emplace_back((std::wstring)L"Filter");
                        buf.emplace_back((std::wstring)header);
                        bool isFiltered = isHeaderFiltered(buf, lvFilters);
                        if (!isFiltered)
                        {
                            lvFilters->add_item(buf);
                        }
                    }

                }
                break;
                case removeFilterID:
                {
                    int iPos = ListView_GetNextItem(lvFilters->Get_Hwnd(), -1, LVNI_SELECTED);
                    //iterating through all selected items
                    while (iPos != -1)
                    {
                        // iPos is the index of a selected item

                        //getHeader based on iPos(index)

                        std::vector<std::wstring> buf;
                        std::wstring type = lvFilters->m_v[iPos][0];
                        std::wstring header = lvFilters->m_v[iPos][1];
                        buf.emplace_back(type);
                        buf.emplace_back(header);
                        for (size_t i = 0; i < lvFilters->m_v.size(); i++)
                        {
                            if (buf == lvFilters->m_v.at(i))
                            {
                                lvFilters->m_v.erase(lvFilters->m_v.begin() + i);
                                ListView_DeleteItem(lvFilters->Get_Hwnd(), iPos);
                                lvFilters->m_itemcount--;
                                ListView_SetItemCountEx(lvFilters->Get_Hwnd(), lvFilters->m_itemcount, NULL);
                                if (isPipeToDLLConnected)
                                {
                                    if (wcscmp(L"Block", type.c_str()) == 0)
                                    {
                                        unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
                                        WORD wordValue = static_cast<WORD>(ulValue);
                                        Header h;
                                        pipeMessage message;
                                        h.action = 0;
                                        h.header = wordValue;
                                        message.id = bHeader;
                                        message.data = (void*)&h;
                                        pipeToDLL.sendBlockHeaderMessage(message);
                                    }
                                }

                                break;

                            }
                        }
                        //Move to next item
                        iPos = ListView_GetNextItem(lvFilters->Get_Hwnd(), -1, LVNI_SELECTED);
                    }

                }
                break;
                case sendPacketID:
                {
                    std::wstring data = getTextFromBox(packetTextBox->Get_Hwnd(), false);
                    pipeMessage message;
                    Packet p = processPacketFromTextBox(data);
                    message.id = injectOutPacket;
                    message.data = &p;
                    pipeToDLL.sendPacketMessage(message);
                }
                break;
                case recvPacketID:
                {
                    std::wstring data = getTextFromBox(packetTextBox->Get_Hwnd(), false);
                    pipeMessage message;
                    Packet p = processPacketFromTextBox(data);
                    message.id = injectInPacket;
                    message.data = &p;
                    pipeToDLL.sendPacketMessage(message);
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
    lvPackets = new ListView(parentHWND, 0, 0, 600, 400, lvPacketID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);
    lvPackets->setExtendedStyltes(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);

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

    lvFilters = new ListView(parentHWND, 600, 0, 204, 200,lvFiltersID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);
    lvFilters->setExtendedStyltes(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvFilters->add_column(102, L"Type");
    lvFilters->add_column(102, L"header");
    lvFilters->showLV();

    CreateWindow(L"static", L"Header:", WS_VISIBLE | WS_CHILD, 600, 205, 50, 25, parentHWND, NULL, NULL, NULL);
    filterTextBox = new Control(parentHWND, 655, 205, 50, 20, filterTextBoxID, L"edit", L"");
    filterHeader = new Control(parentHWND, 705, 205, 50, 20, filterHeaderID,L"button", L"Filter");
    blockHeader = new Control(parentHWND, 755, 205, 50, 20, blockHeaderID, L"button", L"Block");
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

    hListViewPacketPopUpMenu = CreatePopupMenu();
    InsertMenu(hListViewPacketPopUpMenu, 0, MF_BYPOSITION | MF_STRING, lvPacketsFilterID, L"Filter");
    InsertMenu(hListViewPacketPopUpMenu, 0, MF_BYPOSITION | MF_STRING, lvPacketsBlockID, L"Block");

    hListViewFiltersPopUpMenu = CreatePopupMenu();
    InsertMenu(hListViewFiltersPopUpMenu, 0, MF_BYPOSITION | MF_STRING, removeFilterID, L"Remove");


}  
std::wstring getTextFromBox(HWND boxHwnd, bool RemoveSpaces)
{
    wchar_t packet_text[1024];
    int len = GetWindowTextLength(boxHwnd);
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
    std::wstring text(packet_text);

    return text;
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
bool isHeaderFiltered(std::vector<std::wstring> buf, ListView* lv)
{
    for (size_t i = 0; i < lv->m_v.size(); i++)
    {

        if (buf == lv->m_v.at(i))
        {
            return true;
        }
    }
    return false;
}
void showPopUpMenu(HMENU popUpMenu, HWND mainWindowHWND,LPARAM lowParam)
{
    POINT cursor; // Getting the cursor position
    GetCursorPos(&cursor);
    NMITEMACTIVATE* nmia = (NMITEMACTIVATE*)lowParam;
    if (nmia->iItem != -1)
    {
        TrackPopupMenu(popUpMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor.x, cursor.y, 0, mainWindowHWND, NULL);
    }
}

Packet processPacketFromTextBox(std::wstring data)
{
    std::wistringstream stream(data);
    std::wstring segmentString;
    wchar_t delimiter = L' ';
    Packet p;
    p.callerAddress = 0;
    std::wstring header = data.substr(0, 4);
    p.header = std::stoul(header, 0, 16);
    while (std::getline(stream, segmentString, delimiter))
    {
        segmentString.erase(std::remove_if(segmentString.begin(), segmentString.end(), [](char c) { return c == '\r' || c == '\n'; }), segmentString.end());
        Segment segBuffer;

        if (segmentString.front() == L'"')
        {
            segBuffer.type = encodeStr;
            segmentString = segmentString.substr(1, segmentString.size() - 2);
            std::vector<BYTE> lastSegment = p.segments.at(p.segments.size() - 1).bytes;
            p.segments.pop_back();
            std::vector<BYTE> stringBytes = wideStringToBytes(segmentString);
            std::vector<BYTE> final;
            final.insert(final.end(), lastSegment.begin(), lastSegment.end());
            final.insert(final.end(), stringBytes.begin(), stringBytes.end());

            segBuffer.bytes = final;
            p.segments.emplace_back(segBuffer);
        }
        else if (segmentString.front() == L'<')
        {
            segBuffer.type = encodeBuffer;
            segmentString = segmentString.substr(1, segmentString.size() - 2);
            std::vector<BYTE> bufferBytes;

            for (size_t i = 0; i < segmentString.length(); i += 2) {
                std::wstring subStr = segmentString.substr(i, 2);

                // Convert the string to a BYTE
                BYTE num = std::stoul(subStr, 0, 16);
                bufferBytes.insert(bufferBytes.end(), reinterpret_cast<BYTE*>(&num), reinterpret_cast<BYTE*>(&num) + sizeof(BYTE));
            }
            segBuffer.bytes = bufferBytes;
            p.segments.emplace_back(segBuffer);


        }
        else
        {
            int dataLen = wcslen(segmentString.c_str());
            switch (dataLen)
            {
            case 2:
            {
                segBuffer.type = encode1;
                BYTE b = std::stoul(segmentString, 0, 16);
                std::vector<BYTE> data;
                data.insert(data.end(), reinterpret_cast<BYTE*>(&b), reinterpret_cast<BYTE*>(&b) + sizeof(BYTE));
                segBuffer.bytes = data;

            }
            break;
            case 4:
            {
                segBuffer.type = encode2;
                WORD w = std::stoul(segmentString, 0, 16);
                std::vector<BYTE> data;
                data.insert(data.end(), reinterpret_cast<BYTE*>(&w), reinterpret_cast<BYTE*>(&w) + sizeof(WORD));
                segBuffer.bytes = data;
            }
            break;
            case 8:
            {
                segBuffer.type = encode4;
                DWORD dw = std::stoul(segmentString, 0, 16);
                std::vector<BYTE> data;
                data.insert(data.end(), reinterpret_cast<BYTE*>(&dw), reinterpret_cast<BYTE*>(&dw) + sizeof(DWORD));
                segBuffer.bytes = data;
            }
            break;
            }
            p.segments.emplace_back(segBuffer);


        }
    }
    return p;
}