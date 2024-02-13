#include <vector>
#include <string>
#include "controls.h"
#include "Pipe/Pipe.h"
#include <sstream>
#include "encoding.h"
#include <strsafe.h>
#include "windowsx.h"
//ListView functions
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
void SubclassListView(HWND hwndListView, WNDPROC& g_pfnOrigListViewProc, LONG_PTR subClassLVPacketsData)
{
    // Save the original window procedure
    g_pfnOrigListViewProc = (WNDPROC)SetWindowLongPtr(hwndListView, GWLP_WNDPROC, subClassLVPacketsData);
}
int getListViewItemByCords(HWND listViewHwnd, POINT pt)
{
    LVHITTESTINFO hitTestInfo;
    hitTestInfo.pt = pt;
    return ListView_SubItemHitTest(listViewHwnd, &hitTestInfo);
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
void RemoveFilterItem(std::vector<std::vector<std::wstring>>& m_v, HWND hwnd, int index) {
    m_v.erase(m_v.begin() + index);
    ListView_DeleteItem(hwnd, index);
}
void ProcessBlockTypeAndSendMessage(bool isPipeToDLLConnected, const std::wstring& type, const std::wstring& header, Pipe& pipeToDLL) {
    if (isPipeToDLLConnected && type == L"Block") {
        unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
        WORD wordValue = static_cast<WORD>(ulValue);

        Header h;
        h.action = 0;
        h.header = wordValue;

        pipeMessage message;
        message.id = bHeader;
        message.data = (void*)&h;

        pipeToDLL.sendBlockHeaderMessage(message);
    }
}
void ProcessBlockFilterHeaderCase(const std::wstring& header, HWND parentHWND, ListView* lvFilters, bool isPipeToDLLConnected, Pipe& pipeToDLL, const std::wstring& caseType, std::vector<WORD>& blockedHeaders) {
    int len = header.length();

    bool containSpace = (header.find(L' ') != std::wstring::npos);

    if (len > 4) {
        MessageBox(parentHWND, L"Your header contains more than 2 bytes. Please insert a proper header.", L"Error", MB_OK | MB_ICONSTOP);
    }
    else if (len < 4) {
        MessageBox(parentHWND, L"Your header contains less than 2 bytes. Please insert a proper header.", L"Error", MB_OK | MB_ICONSTOP);
    }
    else {
        std::vector<std::wstring> buf = { caseType, header };
        bool isFiltered = isHeaderFiltered(buf, lvFilters);
        if (!isFiltered) {
            lvFilters->add_item(buf);

            unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
            WORD wordValue = static_cast<WORD>(ulValue);

            if (!isPipeToDLLConnected)
                blockedHeaders.emplace_back(wordValue);
            else {
                pipeMessage message;
                Header h;
                h.action = (caseType == L"Block") ? 1 : 0;
                h.header = wordValue;
                message.id = bHeader;
                message.data = (void*)&h;
                pipeToDLL.sendBlockHeaderMessage(message);
            }
        }
    }
}
void lvFilterBlock(ListView* lvPackets, ListView* lvFilters, const std::wstring& caseType, std::vector<WORD>& blockedHeaders, bool isPipeToDLLConnected, Pipe& pipeToDLL) {
    int iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), -1, LVNI_SELECTED);
    while (iPos != -1) {
        std::wstring header = lvPackets->m_v[iPos][2];

        std::vector<std::wstring> buf = { caseType, header };
        bool isFilteredOrBlocked = isHeaderFiltered(buf, lvFilters);

        if (!isFilteredOrBlocked) {
            std::vector<std::wstring> v = { caseType, header };
            lvFilters->add_item(v);

            if (caseType == L"Block") {
                unsigned long ulValue = wcstoul(header.c_str(), NULL, 16);
                WORD wordValue = static_cast<WORD>(ulValue);

                if (!isPipeToDLLConnected)
                    blockedHeaders.emplace_back(wordValue);
                else {
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
        iPos = ListView_GetNextItem(lvPackets->Get_Hwnd(), iPos, LVNI_SELECTED);
    }
}

//Wstring to Packet functions
void stringSegmentToWstring(std::wstring& segmentString, Packet& p, std::wistringstream& stream) {
    std::vector<BYTE> finalBytes;
    std::vector<BYTE> lastSegment = p.segments.at(p.segments.size() - 1).bytes;
    p.segments.pop_back();
    Segment segBuffer;
    segBuffer.type = encodeStr;

    segmentString = segmentString.substr(1); // Removes the first double quote
    if (segmentString.back() == L'"') {
        segmentString = segmentString.substr(0, segmentString.size() - 1); // Removes the last double quote

        finalBytes.insert(finalBytes.end(), lastSegment.begin(), lastSegment.end());
        if (!segmentString.empty()) {
            std::vector<BYTE> stringBytes = wideStringToBytes(segmentString);
            finalBytes.insert(finalBytes.end(), stringBytes.begin(), stringBytes.end());
        }
    }
    else {
        std::wstring finalString = segmentString;
        while (segmentString.back() != L'"') {
            stream >> segmentString;  // Access the stream here
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
void bufferSegmentToWstring(std::wstring& segmentString, Packet& p) {
    Segment segBuffer;
    segBuffer.type = encodeBuffer;
    segmentString = segmentString.substr(1, segmentString.size() - 2);
    std::vector<BYTE> bufferBytes;

    for (size_t i = 0; i < segmentString.length(); i += 2) {
        std::wstring subStr = segmentString.substr(i, 2);
        BYTE num = std::stoul(subStr, 0, 16);
        bufferBytes.push_back(num);
    }

    segBuffer.bytes = bufferBytes;
    p.segments.emplace_back(segBuffer);
}
void dataSegmentToWstring(std::wstring& segmentString, Packet& p) {
    int dataLen = segmentString.length();
    Segment segBuffer;
    std::vector<BYTE> data;

    switch (dataLen) 
    {
        case 2:
        {
            segBuffer.type = encode1;
            BYTE b = std::stoul(segmentString, 0, 16);
            data.push_back(b);
        }
        break;
        case 4:
        {
            segBuffer.type = encode2;
            WORD w = std::stoul(segmentString, 0, 16);
            data.insert(data.end(), reinterpret_cast<BYTE*>(&w), reinterpret_cast<BYTE*>(&w) + sizeof(WORD));
        }
        break;
        case 8:
        {
            segBuffer.type = encode4;
            DWORD dw = std::stoul(segmentString, 0, 16);
            data.insert(data.end(), reinterpret_cast<BYTE*>(&dw), reinterpret_cast<BYTE*>(&dw) + sizeof(DWORD));
        }
        break;
        default:
            // Handle other cases or provide an error condition if needed
            break;
    }

    segBuffer.bytes = data;
    p.segments.emplace_back(segBuffer);
}
Packet WstringToPacket(std::wstring data) {
    std::wistringstream stream(data);
    std::wstring segmentString;
    wchar_t delimiter = L' ';
    Packet p;
    p.callerAddress = 0;
    std::wstring header = data.substr(0, 4);
    p.header = std::stoul(header, 0, 16);

    while (std::getline(stream, segmentString, delimiter)) {
        if (segmentString.empty()) continue;
        segmentString.erase(std::remove_if(segmentString.begin(), segmentString.end(), [](wchar_t c) {
            return c == L'\r' || c == L'\n';
            }), segmentString.end());

        if (segmentString.front() == L'"') {
            stringSegmentToWstring(segmentString, p, stream);
        }
        else if (segmentString.front() == L'<') {
            bufferSegmentToWstring(segmentString, p);
        }
        else {
            dataSegmentToWstring(segmentString, p);
        }
    }

    return p;
}

//Gui functions
void showPopUpMenu(HMENU popUpMenu, HWND mainWindowHWND, LPARAM lowParam)
{
    POINT cursor; // Getting the cursor position
    GetCursorPos(&cursor);
    NMITEMACTIVATE* nmia = (NMITEMACTIVATE*)lowParam;
    if (nmia->iItem != -1)
    {
        TrackPopupMenu(popUpMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor.x, cursor.y, 0, mainWindowHWND, NULL);
    }
}
POINT getMouseCordsByLP(LPARAM lp)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lp);
    pt.y = GET_Y_LPARAM(lp);
    return pt;
}

//File Dialogs functions
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

//Text boxes functions
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


//ToolTipFunctions
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






//bytesSegment to wstringSegment
std::vector<std::wstring> dataSegToWstringSeg(const std::wstring& type, std::vector<BYTE>& bytes) {
    std::reverse(bytes.begin(), bytes.end());
    std::vector<std::wstring> segString = { type, bytesToWString(bytes) };
    return segString;
}
std::vector<std::wstring> bufferSegToWstringSeg(const std::wstring& type,  const std::vector<BYTE>& bytes) {
    std::vector<std::wstring> segString = { type, bytesToWString(bytes) };
    return segString;
}
std::vector<std::wstring> stringSegToWstringSeg(std::vector<BYTE>& bytes) {
    std::vector<BYTE> stringBytes(bytes.begin() + 2, bytes.end());
    std::vector<BYTE> lengthBytes(bytes.begin(), bytes.begin() + 2);
    std::reverse(lengthBytes.begin(), lengthBytes.end());
    std::wstring finalString = bytesToWString(lengthBytes) + L' ' + L'\"' + bytesToActualString(stringBytes) + L'\"';
    std::vector<std::wstring> segString = { L"STRING", finalString };

    return segString;
}
std::vector<std::wstring> segToWstring(Segment& segment) {
    std::vector<std::wstring> segString;

    switch (segment.type) {
        case decode1:
        case encode1:
            segString = dataSegToWstringSeg(L"BYTE", segment.bytes);
            break;
        case decode2:
        case encode2:
            segString = dataSegToWstringSeg(L"WORD", segment.bytes);
            break;
        case decode4:
        case encode4:
            segString = dataSegToWstringSeg(L"DWORD", segment.bytes);
            break;
        case decodeStr:
        case encodeStr:
            segString = stringSegToWstringSeg(segment.bytes);
            break;
        case decodeBuffer:
        case encodeBuffer:
            segString = bufferSegToWstringSeg(L"BUFFER", segment.bytes);
            break;
        default:
            break;
    }

    return segString;
}
