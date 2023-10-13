#pragma once
template <typename T>
std::vector<BYTE> convertToBytes(T value) {
    std::vector<BYTE> bytes;
    for (int i = 0; i < sizeof(T); ++i) {
        bytes.push_back(static_cast<BYTE>((value >> (8 * i)) & 0xFF));
    }
    std::reverse(bytes.begin(),bytes.end());
    return bytes;
}
