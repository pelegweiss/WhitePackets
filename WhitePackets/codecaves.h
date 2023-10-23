#pragma once
#include "structs.h"
#include "windows.h"


Packet packet;
Packet injectedPacket;

void __fastcall sniff_send(void* ecx, void* edx, COutPacket* p);
bool __fastcall sniff_recv(void* ecx, void* edx,DWORD address, CInPacket* p);

void __fastcall hook_coutpacket(void* ecx, void* edx, DWORD header, DWORD address);
void __fastcall hook_encode1(void* ecx, void* edx, unsigned char content);
void __fastcall hook_encode2(void* ecx, void* edx, unsigned short content);
void __fastcall hook_encode4(void* ecx, void* edx, unsigned long content);
void __fastcall hook_encodestr(void* ecx, void* edx, char * str);
void __fastcall hook_encodebuffer(void* ecx, void* edx, void* ptr,unsigned int len);
void __fastcall hook_decode1(CInPacket* ecx, void* edx, DWORD address);
void __fastcall hook_decode2(CInPacket* ecx, void* edx, DWORD address);
void __fastcall hook_decode4(CInPacket* ecx, void* edx, DWORD address);
void __cdecl hook_decodestr(DWORD address,void* param1, char* str_ptr, unsigned int param2,CInPacket * p);
void __fastcall hook_decodebuffer(CInPacket* ecx, void* edx, DWORD address, void* ptr, unsigned int len);

void my_send_packet();
void my_recv_packet();
void my_coutpacket();
void my_encode1();
void my_encode2();
void my_encode4();
void my_encodestr();
void my_encodebuffer();
void my_decode1();
void my_decode2();
void my_decode4();
void my_decodeStr();
void my_decodeBuffer();

void setUpHooks();
void SendPacket(Packet packet);
void ProcessPacket(Packet data);

