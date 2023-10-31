#include "gui.h"


WNDPROC g_pfnOrigListViewProc = nullptr; // Original ListView window procedure
HWND hwndTooltip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHWND, NULL, NULL, NULL);
LRESULT CALLBACK subClassLVPacketsData(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool tooltipActive = false;
    static int hoveredItem = -1;

    switch (uMsg) {
    case WM_MOUSEMOVE: 
    {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);

        int newItem = getListViewItemByCords(hwnd, pt);

        if (newItem != hoveredItem) 
        {
            // Mouse is not over the previous item, clear the tooltip
            hoveredItem = newItem;

            if (hoveredItem != -1) {
                std::wstring itemType = lvPacketsData->m_v[hoveredItem][0];
                unsigned long long itemValue = 0;

                if (itemType != L"BUFFER") 
                {
                    itemValue = std::stoull(lvPacketsData->m_v[hoveredItem][1], 0, 16);
                }
                
                std::wstring tooltipText;
                if (itemType == L"BYTE") 
                {
                    unsigned char val1 = static_cast<unsigned char>(itemValue); // For BYTE (8 bits)
                    char val2 = static_cast<char>(itemValue); // For BYTE (8 bits)
                    tooltipText = L"Uint8: " + std::to_wstring(val1) + L"\nInt8: " + std::to_wstring(val2);
                }
                else if (itemType == L"WORD") 
                {
                    unsigned short val1 = static_cast<unsigned short>(itemValue); // For WORD (16 bits)
                    short val2 = static_cast<short>(itemValue); // For WORD (16 bits)
                    tooltipText = L"Uint16: " + std::to_wstring(val1) + L"\nInt16: " + std::to_wstring(val2);
                }
                else if (itemType == L"DWORD") 
                {
                    unsigned int val1 = static_cast<unsigned int>(itemValue); // For DWORD (32 bits)
                    int val2 = static_cast<int>(itemValue); // For DWORD (32 bits)
                    tooltipText = L"Uint32: " + std::to_wstring(val1) + L"\nInt32: " + std::to_wstring(val2);
                }
                else if (itemType == L"STRING") 
                    tooltipText = L"The 2 bytes leading the string represent the length of the string";

                TOOLINFO toolInfo = { sizeof(TOOLINFO) };
                toolInfo.cbSize = sizeof(TOOLINFO);
                toolInfo.hwnd = hwnd;
                toolInfo.uFlags = TTF_SUBCLASS;
                toolInfo.uId = 0; // Unique identifier for the item
                toolInfo.lpszText = LPWSTR(tooltipText.c_str());
                toolInfo.rect = { pt.x, pt.y, pt.x + 50, pt.y + 20 }; // Tooltip display area

                enableToolTip(hwndTooltip, toolInfo);
                SetTimer(hwnd, 1, 3000, nullptr);
                tooltipActive = true;
            }
            else 
            {
                if (tooltipActive) 
                {
                    // Deactivate the tooltip if the mouse is not over any item
                    TOOLINFO toolInfo = { sizeof(TOOLINFO) };
                    disableToolTip(hwndTooltip, toolInfo);
                    tooltipActive = false;
                }
            }
        }
    }
    break;
    case WM_TIMER:
    {
        // Deactivate the tooltip after the delay
        if (wParam == 1)
        {
            TOOLINFO toolInfo = { sizeof(TOOLINFO) };
            SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, LPARAM(&toolInfo));
            SendMessage(hwndTooltip, TTM_DELTOOL, 0, LPARAM(&toolInfo));
            tooltipActive = false;
        }
    }
    break;
    default:
        return CallWindowProc(g_pfnOrigListViewProc, hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

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
                    std::wstring bufferPath = open_file(settingsHWND, L"Executables (*.exe)\0*.exe\0");
                    SetWindowText(maplestoryPathTextBox->Get_Hwnd(), bufferPath.c_str());
                    maplestoryPath = bufferPath;
                }
                break;
                case dllPathButtonID:
                {
                    std::wstring bufferPath = open_file(settingsHWND, L"Dynamic Link Library (*.dll)\0*.dll\0");
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
            SetWindowText(maplestoryPathTextBox->Get_Hwnd(), maplestoryPath.c_str());
            SetWindowText(dllPathTextBox->Get_Hwnd(), dllPath.c_str());
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
            LPNMHDR pnmh = (LPNMHDR)lp;

            switch (pnmh->code)
            {

                case LVN_GETINFOTIP:
                {
                    if (wp == lvPacketsDataID)
                    {
                        LPNMLVGETINFOTIP pGetInfoTip = (LPNMLVGETINFOTIP)lp;

                        // Determine the text for the tooltip (customize this as needed)
                        // In this example, the tooltip text is set to "Tooltip for item X"

                        std::wstring type = lvPacketsData->m_v[pGetInfoTip->iItem][0];
                        std::wstring value = lvPacketsData->m_v[pGetInfoTip->iItem][1];
                        std::wstring tooltipText;
                        unsigned long ulongValue = std::stoul(value,0,16);
                        if (wcscmp(L"BYTE", type.c_str()) == 0)
                        {
                            int8_t val1 = static_cast<int8_t>(ulongValue);
                            uint8_t val2 = static_cast<uint8_t>(ulongValue);
                            tooltipText += L"Int8: " + std::to_wstring(val1) + L"\nUInt8: " + std::to_wstring(val2);
                        }
                        if (wcscmp(L"WORD", type.c_str()) == 0)
                        {
                            int16_t val1 = static_cast<int16_t>(ulongValue);
                            uint16_t val2 = static_cast<uint16_t>(ulongValue);
                            tooltipText += L"Int16: " + std::to_wstring(val1) + L"\nUInt16: " + std::to_wstring(val2);
                        }
                        if (wcscmp(L"DWORD", type.c_str()) == 0)
                        {
                            int val1 = static_cast<int>(ulongValue);
                            unsigned int val2 = static_cast<unsigned int>(ulongValue);
                            tooltipText += L"Int32: " + std::to_wstring(val1) + L"\nUInt32: " + std::to_wstring(val2);
                        }
                        // Copy the tooltip text into the structure
                        StringCchCopy(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, tooltipText.c_str());
                    }
    
                }
                break;
                case NM_CUSTOMDRAW:
                {
                    LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lp;
                    RECT iR = { 0 };
                    ListView_GetSubItemRect(lplvcd->nmcd.hdr.hwndFrom, lplvcd->nmcd.dwItemSpec, lplvcd->iSubItem, LVIR_BOUNDS, &iR);
                    iR.left += 1;

                    switch (lplvcd->nmcd.dwDrawStage)
                    {
                        case CDDS_PREPAINT:
                            return CDRF_NOTIFYITEMDRAW;
                        case CDDS_ITEMPREPAINT:
                            return CDRF_NOTIFYSUBITEMDRAW;
                        case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                        {
                            if (wp == (WPARAM)lvFilters->get_ControlID())
                            {
                                HBRUSH hBrushBG;
                                switch ((int)lplvcd->nmcd.dwItemSpec % 2)
                                {
                                case 0:hBrushBG = CreateSolidBrush(RGB(245, 245, 245)); break;
                                default: hBrushBG = CreateSolidBrush(RGB(255, 255, 255)); break;
                                }
                                if (ListView_GetItemState(lvFilters->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED))
                                {
                                    hBrushBG = CreateSolidBrush(RGB(221, 221, 255));
                                }
                                FillRect(lplvcd->nmcd.hdc, &iR, hBrushBG);
                                std::wstring Text = lvFilters->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem];

                                switch (lplvcd->iSubItem)
                                {
                                case 0:draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), Text); break;
                                case 1:draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), Text); break;
                                }
                            }
                            if (wp == (WPARAM)lvPackets->get_ControlID())
                            {

                                HBRUSH hBrushBG;
                                switch ((int)lplvcd->nmcd.dwItemSpec % 2)
                                {
                                case 0:hBrushBG = CreateSolidBrush(RGB(245, 245, 245)); break;
                                default: hBrushBG = CreateSolidBrush(RGB(255, 255, 255)); break;
                                }
                                if (ListView_GetItemState(lvPackets->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED))
                                {
                                    hBrushBG = CreateSolidBrush(RGB(221, 221, 255));
                                }
                                FillRect(lplvcd->nmcd.hdc, &iR, hBrushBG);
                                std::wstring Text = lvPackets->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem];
                                switch (lplvcd->iSubItem)
                                {
                                    case 0:
                                    {
                                        draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 255), Text);
                                    }
                                    break;

                                    case 1:
                                    {

                                        COLORREF color = RGB(255, 0, 0);
                                        if (wcscmp(Text.c_str(), L"Recv") != 0) { color = RGB(11, 102, 35); }
                                        draw_text(lplvcd->nmcd.hdc, iR, color, Text);
                                    }
                                    break;
                                    case 2:
                                    {
                                        draw_text(lplvcd->nmcd.hdc, iR, RGB(102, 0, 204), Text);
                                    }
                                    break;
                                    case 3:
                                    {
                                        std::wistringstream stream(Text);
                                        std::wstring segmentString;
                                        std::vector<std::wstring> data;
                                        while (stream >> segmentString)
                                        {
                                            segmentString.erase(std::remove_if(segmentString.begin(), segmentString.end(), [](char c) { return c == '\r' || c == '\n'; }), segmentString.end());
                                            if (!segmentString.empty())
                                                if (segmentString.front() == L'"' and segmentString.back() == L'"')
                                                {
                                                    std::wstring lastSegment = data.at(data.size() - 1);
                                                    data.pop_back();
                                                    std::wstring finalString = lastSegment + segmentString + L' ';
                                                    data.emplace_back(finalString);
                                                }
                                                else if (segmentString.front() == L'"' && segmentString.back() != L'"')
                                                {
                                                    std::wstring lastSegment = data.at(data.size() - 1);
                                                    data.pop_back();
                                                    std::wstring finalString = lastSegment + segmentString + L' ';
                                                    while (segmentString.back() != L'"')
                                                    {
                                                        stream >> segmentString;
                                                        finalString += L' ' + segmentString + L' ';
                                                    }
                                                    data.emplace_back(finalString);

                                                }
                                                else
                                                {
                                                    std::wstring finalString = segmentString + L' ';
                                                    data.emplace_back(finalString);
                                                }
                                        }
                                        if (!data.empty())
                                        {
                                            data.at(data.size() - 1).pop_back(); //removing last space
                                            COLORREF color = RGB(192, 192, 220);
                                            for (std::wstring segment : data)
                                            {
                                                switch (wcslen(segment.c_str()) - 1)
                                                {
                                                case 2: {color = RGB(242, 168, 24); }break;
                                                case 4: {color = RGB(240, 113, 120); }break;
                                                case 8: {color = RGB(54, 163, 217); }break;
                                                default:
                                                    bool containsQuote = segment.find(L'"') != std::wstring::npos;
                                                    if (containsQuote)
                                                        color = RGB(0, 0, 128);
                                                    else
                                                        color = RGB(0, 0, 0);
                                                    break;
                                                }

                                                draw_text(lplvcd->nmcd.hdc, iR, color, segment);

                                            }
                                        }

                                    }
                                    break;
                                }
                            }
                            if (wp == (WPARAM)lvPacketsData->get_ControlID())
                            {
                                HBRUSH hBrushBG;
                                switch ((int)lplvcd->nmcd.dwItemSpec % 2)
                                {
                                case 0:hBrushBG = CreateSolidBrush(RGB(245, 245, 245)); break;
                                default: hBrushBG = CreateSolidBrush(RGB(255, 255, 255)); break;
                                }
                                if (ListView_GetItemState(lvPacketsData->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED))
                                {
                                    hBrushBG = CreateSolidBrush(RGB(221, 221, 255));
                                }
                                FillRect(lplvcd->nmcd.hdc, &iR, hBrushBG);
                                std::wstring Text = lvPacketsData->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem];

                                switch (lplvcd->iSubItem)
                                {
                                case 0:draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), Text); break;
                                case 1:draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), Text); break;
                                }
                            }
                            return CDRF_SKIPDEFAULT;
                        }
                    }
                }
                break;

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

                                    lvPacketsData->clear_items();
                                    Packet p = processPacketFromWstring(finalString);
                                    for (Segment seg : p.segments)
                                    {
                                        switch (seg.type)
                                        {
                                        case decode1:
                                        case encode1:
                                        {
                                            std::vector<std::wstring> segString;
                                            segString.emplace_back(L"BYTE");
                                            std::reverse(seg.bytes.begin(), seg.bytes.end());
                                            segString.emplace_back(bytesToWString(seg.bytes));
                                            lvPacketsData->add_item(segString);                                        
                                        }
                                        break;
                                        case decode2:
                                        case encode2:
                                        {
                                            std::vector<std::wstring> segString;
                                            segString.emplace_back(L"WORD");
                                            std::reverse(seg.bytes.begin(), seg.bytes.end());
                                            segString.emplace_back(bytesToWString(seg.bytes));
                                            lvPacketsData->add_item(segString);
                                        }
                                        break;
                                        case decode4:
                                        case encode4:
                                        {
                                            std::vector<std::wstring> segString;
                                            segString.emplace_back(L"DWORD");
                                            std::reverse(seg.bytes.begin(), seg.bytes.end());
                                            segString.emplace_back(bytesToWString(seg.bytes));
                                            lvPacketsData->add_item(segString);                                        
                                        }
                                        break;
                                        case decodeStr:
                                        case encodeStr:
                                        {
                                            std::reverse(seg.bytes.begin(), seg.bytes.begin() + 2);
                                            std::vector<std::wstring> segString;
                                            std::vector<BYTE> stringBytes;
                                            std::vector<BYTE> lengthBytes;
                                            lengthBytes.insert(lengthBytes.end(), seg.bytes.begin(), seg.bytes.begin() + 2);
                                            stringBytes.insert(stringBytes.end(), seg.bytes.begin() + 2, seg.bytes.end());
                                            std::wstring finalString;
                                            segString.emplace_back(L"STRING");
                                            finalString += bytesToWString(lengthBytes) + L' ';
                                            finalString += L'\"' + bytesToActualString(stringBytes) + L'\"';
                                            segString.emplace_back(finalString);
                                            lvPacketsData->add_item(segString);                                        
                                        }
                                        break;
                                        case decodeBuffer:
                                        case encodeBuffer:
                                        {
                                            std::vector<std::wstring> segString;
                                            segString.emplace_back(L"BUFFER");
                                            segString.emplace_back(bytesToWString(seg.bytes));
                                            lvPacketsData->add_item(segString);                                        
                                        }
                                        break;
                                        }
                                    }

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
                    if (wcscmp(maplestoryPath.c_str(), L"") == 0)
                    {
                        MessageBox(parentHWND, L"Maplestory path is missing", L"Failed launching", MB_OK | MB_ICONSTOP);
                        break;
                    }
                    if (wcscmp(dllPath.c_str(), L"") == 0)
                    {
                        MessageBox(parentHWND, L"DLL path is missing", L"Failed launching", MB_OK | MB_ICONSTOP);
                        break;
                    }
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
                case saveFiltersID:
                {
                    std::wstring file_path = save_file(parentHWND);
                    if (wcscmp(file_path.c_str(), L"") != 0)
                    {
                        jsonf jsonfile;
                        std::ofstream file(file_path);

                        for (int i = 0; i < lvFilters->m_v.size(); i++)
                        {
                            std::wstring type = lvFilters->m_v[i][0];
                            std::wstring header = lvFilters->m_v[i][1];
                            jsonfile[i]["header"] = header;
                            jsonfile[i]["type"] = type;

                        }
                        file << jsonfile;
                        file.close();
                    }
                }
                break;
                case loadFiltersID:
                {
                    std::wstring json_path = open_file(parentHWND, L"Json (*.json)\0*.json\0");
                    if (wcscmp(json_path.c_str(), L"") != 0)
                    {
                        std::ifstream f(json_path);
                        jsonf jsonfile = jsonf::parse(f);
                        lvFilters->clear_items();
                        for (int i = 0; i < jsonfile.size(); i++)
                        {

                            std::wstring type = jsonfile[i].at("type");
                            std::wstring header = jsonfile[i].at("header");
                            std::vector<std::wstring> buf;
                            buf.emplace_back(type);
                            buf.emplace_back(header);
                            lvFilters->add_item(buf);
                        }
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
            std::wstring json_path = L"./settings.json";
            std::ifstream f(json_path);
            jsonf jsonfile = jsonf::parse(f);
            std::wstring maplestoryPathBuffer = jsonfile["paths"].at("maplestoryPath");
            std::wstring dllPathBuffer = jsonfile["paths"].at("dllPath");
            maplestoryPath = maplestoryPathBuffer;
            dllPath = dllPathBuffer;

            SubclassListView(lvPacketsData->Get_Hwnd());


        }
        break;
        case WM_DESTROY:
        {
            std::wstring file_path = L"./settings.json";
            jsonf jsonfile;
            std::ofstream file(file_path);
            if (file.is_open())
            {
                std::cout << "File is open" << std::endl;
                jsonfile["paths"]["maplestoryPath"] = maplestoryPath;
                jsonfile["paths"]["dllPath"] = dllPath;
                file << jsonfile;
                file.close();
            }
            else
            {
                std::cout << "Something went wrong, couldn't save settings" << std::endl;
            }
            PostQuitMessage(0);
        }
        break;
        default:
            return DefWindowProc(parentHWND, msg, wp, lp);
    }
}
void addControls(HWND parentHWND)
{

    lvPackets = new ListView(parentHWND, 0, 0, 970, 665, lvPacketID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);
    lvPackets->setExtendedStyltes(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);

    lvPackets->add_column(70, L"Caller");
    lvPackets->add_column(40, L"Type");
    lvPackets->add_column(50, L"header");
    lvPackets->add_column(2000, L"Data");
    lvPackets->showLV();

    packetTextBox = new Control(parentHWND, 0, 675, 860, 35, packetTextBoxID, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL);

    sendPacket = new Control(parentHWND, 870, 675, 50, 35, sendPacketID, L"button", L"Send");
    recvPacket = new Control(parentHWND, 920, 675, 50, 35, recvPacketID, L"button", L"Recv");

    sniffPackets = new Control(parentHWND, 0, 710, 50, 25, sniffPacketsID, L"Button", L"Start");
    clearLVPackets = new Control(parentHWND, 55, 710, 50, 25, clearLVPacketsID, L"Button", L"Clear");

    CreateWindow(L"static", L"Auto scroll is: ", WS_VISIBLE | WS_CHILD, 110, 715, 90, 25, parentHWND, NULL, NULL, NULL);
    autoScroll = new Control(parentHWND, 205,710, 25, 25, autoScrollID, L"Button", L"ON");

    lvFilters = new ListView(parentHWND, 975, 0, 204, 200,lvFiltersID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);
    lvFilters->setExtendedStyltes(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvFilters->add_column(102, L"Type");
    lvFilters->add_column(102, L"header");
    lvFilters->showLV();

    CreateWindow(L"static", L"Header:", WS_VISIBLE | WS_CHILD, 975, 205, 50, 25, parentHWND, NULL, NULL, NULL);
    filterTextBox = new Control(parentHWND, 1030, 205, 50, 20, filterTextBoxID, L"edit", L"");
    filterHeader = new Control(parentHWND, 1080, 205, 50, 20, filterHeaderID,L"button", L"Filter");
    blockHeader = new Control(parentHWND, 1130, 205, 50, 20, blockHeaderID, L"button", L"Block");

    lvPacketsData = new ListView(parentHWND, 975, 230, 204, 435, lvPacketsDataID, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);
    lvPacketsData->setExtendedStyltes(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
    lvPacketsData->m_scroll = false;
    lvPacketsData->add_column(102, L"Type");
    lvPacketsData->add_column(500, L"Data");

    launchButton = new Control(parentHWND, 1080, 690, 100, 50, launchButtonID, L"button", L"Launch");
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
    filterMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)filterMenu, L"Filters");
    AppendMenu(filterMenu, MF_STRING, saveFiltersID, L"Save");
    AppendMenu(filterMenu, MF_STRING, loadFiltersID, L"Load");
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
std::wstring save_file(HWND hWnd)
{
    WCHAR currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    OPENFILENAME ofn = { 0 };

    TCHAR szFileName[512] = { 0 };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"json\0*.json\0";
    ofn.lpstrFile = (LPWSTR)szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        SetCurrentDirectory(currentDirectory);
        return std::wstring(ofn.lpstrFile);

    }
    return L"";
}
std::wstring open_file(HWND hWnd, const wchar_t* fileFilter)
{
    WCHAR currentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDirectory);
    OPENFILENAME ofn = { 0 };
    wchar_t szFile[MAX_PATH] = { 0 };

    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = fileFilter;  // Specify the filter for allowed file types

    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        SetCurrentDirectory(currentDirectory);
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
Packet processPacketFromWstring(std::wstring data)
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
        if (segmentString.empty())
            continue;
        segmentString.erase(std::remove_if(segmentString.begin(), segmentString.end(), [](char c) { return c == '\r' || c == '\n'; }), segmentString.end());
        Segment segBuffer;

        if (segmentString.front() == L'"') //its a string
        {
            std::vector<BYTE> finalBytes;
            std::vector<BYTE> lastSegment = p.segments.at(p.segments.size() - 1).bytes;
            p.segments.pop_back();
            segBuffer.type = encodeStr;
            segmentString = segmentString.substr(1); //removes the first double mark qouate
            if (segmentString.back() == L'"')
            {
                segmentString = segmentString.substr(0, segmentString.size() - 1); //remove the last double mark qouate 

                finalBytes.insert(finalBytes.end(), lastSegment.begin(), lastSegment.end());
                if (!segmentString.empty())
                {
                    std::vector<BYTE> stringBytes = wideStringToBytes(segmentString);
                    finalBytes.insert(finalBytes.end(), stringBytes.begin(), stringBytes.end());
                }

            }
            else
            {
                    std::wstring finalString = segmentString;
                    while (segmentString.back() != L'"')
                    {
                        stream >> segmentString;
                        finalString += L' ' + segmentString;
                    }
                    finalString = finalString.substr(0, finalString.length() - 1);
                    std::vector<BYTE> stringBytes = wideStringToBytes(finalString);
                    finalBytes.insert(finalBytes.end(), lastSegment.begin(), lastSegment.end());
                    finalBytes.insert(finalBytes.end(), stringBytes.begin(), stringBytes.end());

            }
            segBuffer.bytes = finalBytes;
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
            if (segmentString.empty())
            {
                segmentString = L"\"\"";
            }
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
void draw_text(HDC hdc, RECT& iR, COLORREF color, std::wstring Text) {
    SIZE sz = { 0 };
    GetTextExtentPoint32(hdc, Text.c_str(), Text.length(), &sz);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawText(hdc, Text.c_str(), Text.length(), &iR, DT_LEFT);
    Text += L" ";
    SIZE new_sz = { 0 };
    GetTextExtentPoint32(hdc, Text.c_str(), Text.length(), &new_sz);
    iR.left += sz.cx;
}

void SubclassListView(HWND hwndListView)
{
    // Save the original window procedure
    g_pfnOrigListViewProc = (WNDPROC)SetWindowLongPtr(hwndListView, GWLP_WNDPROC, (LONG_PTR)subClassLVPacketsData);
}

int getListViewItemByCords(HWND listViewHwnd,POINT pt)
{
    LVHITTESTINFO hitTestInfo;
    hitTestInfo.pt = pt;
    return ListView_SubItemHitTest(listViewHwnd, &hitTestInfo);
}



void enableToolTip(HWND hwndToolTip, TOOLINFO toolInfo)
{
    SendMessage(hwndToolTip, TTM_SETMAXTIPWIDTH, 0, 300); // Adjust the width as needed
    SendMessage(hwndToolTip, TTM_ADDTOOL, 0, LPARAM(&toolInfo));
    SendMessage(hwndToolTip, TTM_TRACKACTIVATE, TRUE, LPARAM(&toolInfo));
    SendMessage(hwndToolTip, TTM_UPDATETIPTEXT, 0, LPARAM(&toolInfo));
}
void disableToolTip(HWND hwndToolTip, TOOLINFO toolInfo)
{
    SendMessage(hwndToolTip, TTM_TRACKACTIVATE, FALSE, LPARAM(&toolInfo));
    SendMessage(hwndToolTip, TTM_DELTOOL, 0, LPARAM(&toolInfo));
}