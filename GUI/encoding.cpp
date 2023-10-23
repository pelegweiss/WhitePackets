#include <vector>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "Pipe/Pipe.h"

std::wstring toHexString(const std::vector<unsigned char>& bytes) {
    std::wstringstream ss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase << static_cast<int>(bytes[i]);
    }
    return ss.str();
}
std::wstring bytesToWString(const std::vector<unsigned char>& bytes) {
    std::wstringstream wss;
    for (unsigned char byte : bytes) {
        wss << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase << static_cast<int>(byte);
    }
    return wss.str();
}
std::wstring bytesToActualString(const std::vector<unsigned char>& bytes) {
    std::wstring result;
    for (unsigned char byte : bytes) {
        result += static_cast<wchar_t>(byte);
    }
    return result;
}

std::vector<unsigned char> wideStringToBytes(const std::wstring& wideStr) {
    std::vector<unsigned char> bytes;

    for (wchar_t wideChar : wideStr) 
    {
        // Convert each wide character to a byte and append it to the vector
        bytes.push_back(static_cast<unsigned char>(wideChar));
    }

    return bytes;
}
std::wstring segmentsToWstring(const std::vector<Segment>& segments) {

    std::wstring data;
    for (const Segment& segment : segments) {
        std::wstring hexString;
        if (segment.type == encodeStr || segment.type == decodeStr)
        {
            std::vector<BYTE> length(segment.bytes.begin(), segment.bytes.begin() + 2);
            std::vector<BYTE> bytesBuffer(segment.bytes.begin() + 2, segment.bytes.end());
            hexString += bytesToWString(length);
            hexString += L" ";
            hexString += L"\"" + bytesToActualString(bytesBuffer) + L"\"";
        }
        else if (segment.type == encodeBuffer || segment.type == decodeBuffer)
        {
            hexString += L"<" + toHexString(segment.bytes) + L">";
        }
        else
        {
            hexString = toHexString(segment.bytes);
        }

        if (!data.empty()) {
            data += L" ";
        }
        data += hexString;
    }
    return data;
}
