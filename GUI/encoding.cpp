#include <vector>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <sstream>
#include "Pipe/Pipe.h"

std::wstring toHexString(const std::vector<BYTE>& bytes) {
    std::wstringstream ss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase << static_cast<int>(bytes[i]);
    }
    return ss.str();
}
std::wstring bytesToWString(const std::vector<unsigned char>& bytes) {
    std::wstring result;
    for (unsigned char byte : bytes) {
        result += static_cast<wchar_t>(byte);
    }
    return result;
}

std::wstring segmentsToWstring(const std::vector<Segment>& segments) {
    std::wstring data;
    for (const Segment& segment : segments) {
        std::wstring hexString = toHexString(segment.bytes);

        if (segment.type == encodeStr) {
            hexString.insert(4, 1, L' ');
            std::vector<BYTE> bytesBuffer(segment.bytes.begin() + 2, segment.bytes.end());
            hexString += L"[\"" + bytesToWString(bytesBuffer) + L"\"]";
        }

        if (!data.empty()) {
            data += L" ";
        }
        data += hexString;
    }
    return data;
}
