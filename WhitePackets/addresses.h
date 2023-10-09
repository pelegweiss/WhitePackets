#pragma once
#include <Windows.h>
#define CClientSocket_SendPacketAddress 0x49637B
#define CClientSocket_ProcessPacketAddress 0x4965F1
#define CClientSocket_SingletonAddress 0x00BE7914

#define COutPacket_COutPacketAddress 0x006EC9CE

#define COutPacket_Encode1Address 0x00406549
#define COutPacket_Encode2Address 0x00427F74
#define COutPacket_Encode4Address 0x004065A6
#define COutPacket_EncodeStrAddress 0x0046F3CF
#define COutPacket_EncodeBufferAddress 0x0046C00C

#define CInPacket_Decode1Address 0x004065F3
#define CInPacket_Decode2Address 0x0042470C
#define CInPacket_Decode4Address 0x00406629
#define CInPacket_DecodeStrAddress 0x0046F37B
#define CInPacket_DecodeBufferAddress 0x00432257


DWORD jmpbackAddy_send = (DWORD)CClientSocket_SendPacketAddress + 5;
DWORD jmpbackAddy_recv = (DWORD)CClientSocket_ProcessPacketAddress + 5;
DWORD jmpbackAddy_CoutPacket = (DWORD)COutPacket_COutPacketAddress + 5;
DWORD jmpbackAddy_Enocde1 = (DWORD)COutPacket_Encode1Address + 5;
DWORD jmpbackAddy_Encode2 = (DWORD)COutPacket_Encode2Address + 5;
DWORD jmpbackAddy_Encode4 = (DWORD)COutPacket_Encode4Address + 5;
DWORD jmpbackAddy_EncodeStr = (DWORD)COutPacket_EncodeStrAddress + 5;
DWORD jmpbackAddy_EncodeBuffer = (DWORD)COutPacket_EncodeBufferAddress + 6;

DWORD jmpbackAddy_Decode1 = (DWORD)CInPacket_Decode1Address + 7;
DWORD jmpbackAddy_Decode2 = (DWORD)CInPacket_Decode2Address + 7;
DWORD jmpbackAddy_Decode4 = (DWORD)CInPacket_Decode4Address + 7;
DWORD jmpbackAddy_DecodeStr = (DWORD)CInPacket_DecodeStrAddress + 7;
DWORD jmpbackAddy_DecodeBuffer = (DWORD)CInPacket_DecodeBufferAddress + 6;