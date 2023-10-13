#pragma once
#include <vector>
#include <Windows.h>
#include <string>
#include "encoding.tpp"
std::wstring toHexString(const std::vector<BYTE>& bytes);
std::wstring bytesToWString(const std::vector<unsigned char>& bytes);
std::wstring segmentsToWstring(const std::vector<Segment>& segments);