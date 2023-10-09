#pragma once
#include "structs.h"
#include "windows.h"


void __fastcall sniff_send(void* ecx, void* edx, DWORD address, COutPacket* p);
bool __fastcall sniff_recv(void* ecx, void* edx,DWORD address, CInPacket* p);

void __fastcall hook_coutpacket(void* ecx, void* edx, short header);
void __fastcall hook_encode1(void* ecx, void* edx, unsigned char content);
void __fastcall hook_encode2(void* ecx, void* edx, unsigned short content);
void __fastcall hook_encode4(void* ecx, void* edx, unsigned long content);
void __fastcall hook_encodestr(void* ecx, void* edx, char * str);
void __fastcall hook_encodebuffer(void* ecx, void* edx, void* ptr,unsigned int len);
void __fastcall hook_decode1(void* ecx, void* edx, DWORD address);
void __fastcall hook_decode2(void* ecx, void* edx, DWORD address);
void __fastcall hook_decode4(void* ecx, void* edx, DWORD address);
void __cdecl hook_decodestr(void* param_1, DWORD address, char* str, unsigned int param_3);
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

