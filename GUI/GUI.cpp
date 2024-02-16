#include "gui.h"
#include <filesystem>


LRESULT CALLBACK lvPacketsDataProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool tooltipActive = false;
    static int hoveredItem = -1;

    switch (uMsg) {
    case WM_MOUSEMOVE: 
    {

        POINT pt = getMouseCordsByLP(lParam);
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

                enableToolTip(lvPacketsDataToolTip->Get_Hwnd(), toolInfo);
                SetTimer(hwnd, 1, 3000, nullptr);
                tooltipActive = true;
            }
            else 
            {
                if (tooltipActive) 
                {
                    // Deactivate the tooltip if the mouse is not over any item
                    TOOLINFO toolInfo = { sizeof(TOOLINFO) };
                    disableToolTip(lvPacketsDataToolTip->Get_Hwnd(), toolInfo);
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
            disableToolTip(lvPacketsDataToolTip->Get_Hwnd(), toolInfo);
            tooltipActive = false;
        }
    }
    break;
    default:
        return CallWindowProc(ogLvPacketsDataProcedure, hwnd, uMsg, wParam, lParam);
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
        switch (((NMHDR*)lp)->code)
        {
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
                    HBRUSH hBrushBG;
                    switch ((int)lplvcd->nmcd.dwItemSpec % 2)
                    {
                        case 0:hBrushBG = CreateSolidBrush(RGB(245, 245, 245)); break;
                        default: hBrushBG = CreateSolidBrush(RGB(255, 255, 255)); break;
                    }
                    if (ListView_GetItemState(lvFilters->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED) || ListView_GetItemState(lvPackets->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED) || ListView_GetItemState(lvPacketsData->Get_Hwnd(), lplvcd->nmcd.dwItemSpec, LVIS_SELECTED))
                    {
                        hBrushBG = CreateSolidBrush(RGB(221, 221, 255));
                    }
                    FillRect(lplvcd->nmcd.hdc, &iR, hBrushBG);
                    switch (wp)
                    {
                        case lvPacketsDataID:
                        {
                            draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), lvPacketsData->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem]);

                        }
                        break;
                        case lvFiltersID:
                        {
                            draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 0), lvFilters->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem]);
                        }
                        break;
                        case lvPacketID:
                        {
                            std::wstring Text = lvPackets->m_v[lplvcd->nmcd.dwItemSpec][lplvcd->iSubItem];
                            switch (lplvcd->iSubItem)
                            {
                                case 0:
                                case 2:
                                {
                                    draw_text(lplvcd->nmcd.hdc, iR, RGB(0, 0, 255), Text);
                                }
                                break;
                                case 1:
                                {

                                    COLORREF color;
                                    Text == L"Recv" ? color = RGB(255, 0, 0) : color = RGB(11, 102, 35);
                                    draw_text(lplvcd->nmcd.hdc, iR, color, Text);
                                }
                                break;                            
                                case 3:
                                {
                                    Packet p;
                                    if (!Text.empty())
                                    {
                                        p = WstringToPacket(Text);
                                        for (Segment seg : p.segments)
                                        {
                                            std::vector<std::wstring> item = segToWstring(seg);
                                            COLORREF color = RGB(192, 192, 220);
                                            if (item[0] == L"BYTE")
                                                color = RGB(242, 168, 24);
                                            else if (item[0] == L"WORD")
                                                color = RGB(240, 113, 120);
                                            else if (item[0] == L"DWORD")
                                                color = RGB(54, 163, 217);
                                            else if (item[0] == L"STRING")
                                                color = RGB(0, 0, 128);
                                            else
                                                color = RGB(0, 0, 0);
                                            draw_text(lplvcd->nmcd.hdc, iR, color, item[1] + L" ");
                                        }
                                    }
                                    else
                                        break;

                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                return CDRF_SKIPDEFAULT;

            }
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
                    int iPos = nmia->iItem;// iPos is the index of a selected item
                    if (iPos != -1)
                    {
                        // Set packet TextBox
                        std::wstring header = lvPackets->m_v[iPos][2];
                        std::wstring data = lvPackets->m_v[iPos][3];
                        std::wstring finalString = header + L" " + data;
                        SetWindowText(packetTextBox->Get_Hwnd(), (LPWSTR)finalString.c_str());

                        //set lvPacketData listview
                        lvPacketsData->clear_items();
                        Packet p = WstringToPacket(finalString);
                        for (Segment seg : p.segments) {
                            std::vector<std::wstring> item = segToWstring(seg);
                            lvPacketsData->add_item(item);
                        }

                       
                    }


                }
                break;
            }
            break;
        }
        break;
        case NM_RCLICK:
        {
            switch (wp)
            {
                case lvPacketID:
                {
                    showPopUpMenu(hListViewPacketPopUpMenu, parentHWND, lp);
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
                // Check if paths are missing
                if (maplestoryPath.empty()) {
                    MessageBox(parentHWND, L"Maplestory path is missing", L"Failed launching", MB_OK | MB_ICONSTOP);
                    break;
                }

                if (dllPath.empty()) {
                    MessageBox(parentHWND, L"DLL path is missing", L"Failed launching", MB_OK | MB_ICONSTOP);
                    break;
                }

                // Attempt to launch and inject Maplestory
                const int maxRetries = 3;
                int retriesCounter = 0;
                bool launched = false;

                while (retriesCounter < maxRetries) {
                    if (!runMaplestory(maplestoryPath, dllPath)) 
                    {
                        retriesCounter++;
                    }
                    else {
                        std::cout << "Managed to launch and inject" << std::endl;
                        retriesCounter = maxRetries; // Exit the loop
                        launched = true;
                    }
                }

                if (!launched) {
                    std::cout << "Failed to launch and inject" << std::endl;
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
                lvFilterBlock(lvPackets, lvFilters, L"Filter", blockedHeaders, isPipeToDLLConnected, pipeToDLL);
            }
            break;
            case lvPacketsBlockID: 
            {
                lvFilterBlock(lvPackets, lvFilters, L"Block", blockedHeaders, isPipeToDLLConnected, pipeToDLL);
            }
            break;
            case blockHeaderID: 
            {
                std::wstring header = getTextFromBox(filterTextBox->Get_Hwnd(), false);
                if (!header.empty())
                    ProcessBlockFilterHeaderCase(header, parentHWND, lvFilters, isPipeToDLLConnected, pipeToDLL, L"Block", blockedHeaders);
            }
            break;
            case filterHeaderID: 
            {
                std::wstring header = getTextFromBox(filterTextBox->Get_Hwnd(), false);
                if (!header.empty())
                    ProcessBlockFilterHeaderCase(header, parentHWND, lvFilters, isPipeToDLLConnected, pipeToDLL, L"Filter", blockedHeaders);
            }
            break;
            case removeFilterID:
            {
                int iPos = ListView_GetNextItem(lvFilters->Get_Hwnd(), -1, LVNI_SELECTED);
                while (iPos != -1)
                {
                    std::wstring type = lvFilters->m_v[iPos][0];
                    std::wstring header = lvFilters->m_v[iPos][1];

                    // Create a vector to compare
                    std::vector<std::wstring> buf = { type, header };

                    for (size_t i = 0; i < lvFilters->m_v.size(); i++)
                    {
                        if (buf == lvFilters->m_v[i])
                        {
                            RemoveFilterItem(lvFilters->m_v, lvFilters->Get_Hwnd(), i);
                            lvFilters->m_itemcount--;
                            ListView_SetItemCountEx(lvFilters->Get_Hwnd(), lvFilters->m_itemcount, NULL);

                            if (isPipeToDLLConnected)
                            {
                                ProcessBlockTypeAndSendMessage(isPipeToDLLConnected, type, header, pipeToDLL);
                            }
                            else
                            {
                                unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
                                WORD wordValue = static_cast<WORD>(ulValue);
                                blockedHeaders.erase(std::remove(blockedHeaders.begin(), blockedHeaders.end(), wordValue));
                            }

                            break;
                        }
                    }
                    iPos = ListView_GetNextItem(lvFilters->Get_Hwnd(), -1, LVNI_SELECTED);
                }
            }
            break;
            case sendPacketID:
            {
                std::wstring data = getTextFromBox(packetTextBox->Get_Hwnd(), false);
                pipeMessage message;
                Packet p = WstringToPacket(data);//processPacketFromTextBox(data);
                message.id = outGoingPacket;
                message.data = &p;
                pipeToDLL.sendPacketMessage(message);
            }
            break;
            case recvPacketID:
            {
                std::wstring data = getTextFromBox(packetTextBox->Get_Hwnd(), false);
                pipeMessage message;
                Packet p = WstringToPacket(data);//processPacketFromTextBox(data);
                message.id = inGoingPacket;
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
        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path settingsFilePath = exePath / "settings.json";
        if (std::filesystem::exists(settingsFilePath)) {
            std::cout << "settings.json file found!" << std::endl;
            std::ifstream f(settingsFilePath);
            jsonf jsonfile = jsonf::parse(f);
            std::wstring maplestoryPathBuffer = jsonfile["paths"].at("maplestoryPath");
            std::wstring dllPathBuffer = jsonfile["paths"].at("dllPath");
            maplestoryPath = maplestoryPathBuffer;
            dllPath = dllPathBuffer;
        }
        else {
            std::cout << "settings.json file not found." << std::endl;
            // Add your logic here for when the file does not exist.
        }


        SubclassListView(lvPacketsData->Get_Hwnd(), ogLvPacketsDataProcedure, (LONG_PTR)lvPacketsDataProcedure);
    }
    break;
    case WM_DESTROY:
    {
        // Define the file path
        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path settingsFilePath = exePath / "settings.json";
        // Create JSON object and output file
        jsonf jsonfile;
        std::ofstream file(settingsFilePath);

        if (!file.is_open())
        {
            // Display an error message if file opening fails
            MessageBoxW(parentHWND, L"Something went wrong, couldn't save settings", L"Error", MB_OK | MB_ICONWARNING);

            // Terminate the application
            PostQuitMessage(0);
        }
        // Assign values to the JSON object
        jsonfile["paths"]["maplestoryPath"] = maplestoryPath;
        jsonfile["paths"]["dllPath"] = dllPath;

        // Write JSON data to the file
        file << jsonfile;

        // Close the file
        file.close();

        //kill Maplestory
        system("taskkill /f /im HeavenMS-localhost-WINDOW.exe");

        // Terminate the application
        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(parentHWND, msg, wp, lp);
    }
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



void addControls(HWND parentHWND)
{
    //Adding all controls
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

    lvPacketsDataToolTip = new Control(parentHWND, 0, 0, 0, 0, NULL, TOOLTIPS_CLASSW, NULL, WS_POPUP | TTS_ALWAYSTIP);
}
void addSettingsControl(HWND hwnd)
{
    //adding all controls for settings window
    CreateWindow(L"static", L"Maplestory Path:", WS_VISIBLE | WS_CHILD, 15, 15, 110, 25, hwnd, NULL, NULL, NULL);
    CreateWindow(L"static", L"DLL Path:", WS_VISIBLE | WS_CHILD, 60, 45, 100, 25, hwnd, NULL, NULL, NULL);
    maplestoryPathTextBox = new Control(hwnd, 130, 10, 250, 25, maplestoryPathTextBoxID, L"edit", maplestoryPath.c_str(), ES_READONLY | WS_BORDER | WS_CHILD | WS_VISIBLE);
    dllPathTextBox = new Control(hwnd, 130, 40, 250, 25, dllPathTextBoxID, L"edit", dllPath.c_str(), ES_READONLY | WS_BORDER | WS_CHILD | WS_VISIBLE);
    maplestoryPathButton = new Control(hwnd, 380, 10, 25, 25, maplestoryPathButtonID, L"Button", L"...");
    dllPathButton = new Control(hwnd, 380, 40, 25, 25, dllPathButtonID, L"Button", L"...");

}
void addMenus(HWND parenthWnd)
{
    //adding menus to main window
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


