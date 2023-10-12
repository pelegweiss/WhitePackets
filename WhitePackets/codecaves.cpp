#include <iostream>
#include <Windows.h>
#include "structs.h"
#include "hook.h"
#include "addresses.h"
#include <vector>
//#include "Pipe/Pipe.h"
#include "Pipe.h"
extern Pipe pipeToGui;
Packet * packet;

void __fastcall hook_coutpacket(void* ecx, void* edx, DWORD header, DWORD address)
{
	packet = new Packet;
	packet->callerAddress = address;
	packet->header = header;
}
void __fastcall hook_encode1(void* ecx, void* edx, unsigned char content)
{
	std::vector<BYTE> buffer;
	buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(&content), reinterpret_cast<const BYTE*>(&content) + sizeof(unsigned char));
	std::reverse(buffer.begin(), buffer.end());
	Segment seg;
	seg.bytes = buffer;
	seg.type = encode1;
	packet->segments.emplace_back(seg);
}
void __fastcall hook_encode2(void* ecx, void* edx, unsigned short content)
{
	std::vector<BYTE> buffer;
	buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(&content), reinterpret_cast<const BYTE*>(&content) + sizeof(unsigned short));
	std::reverse(buffer.begin(), buffer.end());
	Segment seg;
	seg.bytes = buffer;
	seg.type = encode2;
	packet->segments.emplace_back(seg);
}
void __fastcall hook_encode4(void* ecx, void* edx, unsigned long content)
{
	std::vector<BYTE> buffer;
	buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(&content), reinterpret_cast<const BYTE*>(&content) + sizeof(unsigned long));
	std::reverse(buffer.begin(), buffer.end());
	Segment seg;
	seg.bytes = buffer;
	seg.type = encode4;
	packet->segments.emplace_back(seg);
}
void __fastcall hook_encodestr(void* ecx, void* edx, char * content)
{
	WORD strLen = strlen(content);
	std::vector<BYTE> buffer;
	buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(&strLen), reinterpret_cast<const BYTE*>(&strLen) + sizeof(WORD));
	std::reverse(buffer.begin(), buffer.end());

	if (strLen != 0)
	{
		buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(content), reinterpret_cast<const BYTE*>(content) + strLen);
	}

	Segment seg;
	seg.bytes = buffer;
	seg.type = encodeStr;
	packet->segments.emplace_back(seg);
}


	





void __fastcall hook_encodebuffer(void* ecx, void* edx, void * ptr, unsigned int len)
{
	std::vector<BYTE> buffer;


	buffer.insert(buffer.end(), reinterpret_cast<const BYTE*>(ptr), reinterpret_cast<const BYTE*>(ptr) + len);

	Segment seg;
	seg.bytes = buffer;
	seg.type = encodeBuffer;
	packet->segments.emplace_back(seg);
}

void __fastcall hook_decode1(void* ecx, void* edx, DWORD address)
{
	std::cout << "Been in decode1" << std::endl;

}
void __fastcall hook_decode2(void* ecx, void* edx, DWORD address)
{
	std::cout << "Been in decode2" << std::endl;
}
void __fastcall hook_decode4(void* ecx, void* edx, DWORD address)
{
	std::cout << "Been in decode4" << std::endl;
}
void __cdecl hook_decodestr(void* param_1,DWORD address, char* str_ptr, unsigned int param_3)
{
	std::cout << "Been in decodeStr" << std::endl;

}
void __fastcall hook_decodebuffer(CInPacket* ecx, void* edx,DWORD address, void* ptr, unsigned int len)
{
	std::cout << "Been in decodeBuffer" << std::endl;
}


void __fastcall sniff_send(void* ecx, void* edx,DWORD address, COutPacket* p)
{
	pipeMessage message;
	message.id = 1;
	message.data = *packet;
	
	pipeToGui.sendMessage(message);
	delete packet;
}

bool __fastcall sniff_recv(void* ecx, void* edx, DWORD address, CInPacket* p)
{
	return false;	
}
void __declspec(naked) my_send_packet()
{
	


	__asm
	{

		pushad
		push[esp + 4 + 8 * 4]
		push[esp + 4 + 8 * 4]
		call sniff_send // my function
		popad
		mov eax, 0x00A8126C // original bytes
		jmp[jmpbackAddy_send] // jump back to original code after running stolen bytes

	}
}


 void __declspec(naked) my_recv_packet()
{

	__asm
	{
		pushad
		push[esp + 4 + 8 * 4]
		push[esp + 8 * 4]
		call sniff_recv // my function
		cmp al, 1
		je block //return and block the packet
		popad
		mov eax, 0x00A812B0 // original bytes
		jmp[jmpbackAddy_recv] // jump back to original code after running stolen bytes

	}
block:
	__asm {
		popad
		ret 4
	}
}


 void __declspec(naked) my_coutpacket()
 {
	 __asm
	 {

		 pushad
		 push[esp + 0x20]
		 push[esp+0x28]
		 call hook_coutpacket // my function
		 popad
		 mov eax, 0x00AABB1F // original bytes
		 jmp[jmpbackAddy_CoutPacket] // jump back to original code after running stolen bytes

	 }
 }
void __declspec(naked) my_encode1()
 {
	 __asm
	 {

		 pushad
		 push[esp + 4 + 8 * 4]
		 call hook_encode1 // my function
		 popad
		 push esi // original bytes
		 mov esi,ecx // original bytes
		 push 01 // original bytes
		 jmp[jmpbackAddy_Enocde1] // jump back to original code after running stolen bytes
	 }
 }
void __declspec(naked) my_encode2()
{
	__asm
	{

		pushad
		push[esp + 4 + 8 * 4]
		call hook_encode2 // my function
		popad
		push esi // original bytes
		mov esi, ecx // original bytes
		push 02 // original bytes
		jmp[jmpbackAddy_Encode2] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_encode4()
{
	__asm
	{

		pushad
		push[esp + 4 + 8 * 4]
		call hook_encode4 // my function
		popad
		push esi // original bytes
		mov esi, ecx // original bytes
		push 04 // original bytes
		jmp[jmpbackAddy_Encode4] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_encodestr()
{
	__asm
	{

		pushad
		push[esp + 4 + 8 * 4]
		call hook_encodestr // my function
		popad
		mov eax,0x00A7E500 // original bytes
		jmp[jmpbackAddy_EncodeStr] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_encodebuffer()
{
	__asm
	{

		pushad
		push[esp + 8 + 8 * 4]
		push[esp + 8 + 8 * 4]
		call hook_encodebuffer // my function
		popad
		//OG Bytes
		push esi
		push edi
		mov edi,[esp+0x10]
		jmp[jmpbackAddy_EncodeBuffer] // jump back to original code after running stolen bytes
	}
}

void __declspec(naked) my_decode1()
{
	__asm
	{
		pushad
		push[esp + 8 * 4]
		call hook_decode1 // my function
		popad
		push ebp // original bytes
		mov ebp, esp // original bytes
		push ecx // original bytes
		mov edx,[ecx + 0x14] // original bytes
		jmp[jmpbackAddy_Decode1] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_decode2()
{
	__asm
	{
		pushad
		push[esp + 8 * 4]
		call hook_decode2 // my function
		popad
		push ebp // original bytes
		mov ebp, esp // original bytes
		push ecx // original bytes
		mov edx, [ecx + 0x14] // original bytes
		jmp[jmpbackAddy_Decode2] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_decode4()
{
	__asm
	{
		pushad
		push[esp + 8 * 4]
		call hook_decode4 // my function
		popad
		push ebp // original bytes
		mov ebp, esp // original bytes
		push ecx // original bytes
		mov edx, [ecx + 0x14] // original bytes
		jmp[jmpbackAddy_Decode4] // jump back to original code after running stolen bytes
	}
}
void __declspec(naked) my_decodeStr()
{
	__asm
	{
		pushad
		push[esp + 12 + 8 * 4]
		push[esp + 12 + 8 * 4]
		push[esp + 12 + 8 * 4]
		push[esp + 12 + 8 * 4] // caller address of the parent function
		call hook_decodestr // my function
		add esp, 16 //cleaning the stack since its cdecl calling convention
		popad // restoring the registers
		push ebp // original bytes
		mov ebp, esp // original bytes
		cmp dword ptr[ebp + 0x10],0x02 // original bytes
		jmp[jmpbackAddy_DecodeStr] // jump back to original code after running stolen bytes
	}
}

void __declspec(naked) my_decodeBuffer()
{
	__asm
	{
		pushad
		push[esp + 8 + 8 * 4]
		push[esp + 8 + 8 * 4]
		push[esp + 8 + 8 * 4]
		call hook_decodebuffer // my function
		popad
		push ebp // original bytes
		mov ebp, esp // original bytes
		push esi // original bytes
		mov esi, ecx // original bytes
		jmp[jmpbackAddy_DecodeBuffer] // jump back to original code after running stolen bytes
	}
}

 void ProcessPacket(wchar_t* data)
 {

 }

 void SendPacket(wchar_t* data)
 {

 }

 void setUpHooks()
 {
	 Hook * sendPacketHook = new Hook((BYTE*)CClientSocket_SendPacketAddress, (BYTE*)&my_send_packet, 5);
	 Hook * recvPacketHook = new Hook((BYTE*)CClientSocket_ProcessPacketAddress, (BYTE*)&my_recv_packet, 5);

	 Hook * CoutPacketHook = new Hook((BYTE*)COutPacket_COutPacketAddress, (BYTE*)&my_coutpacket, 5);
	 Hook * Encode1Hook = new Hook((BYTE*)COutPacket_Encode1Address, (BYTE*)&my_encode1, 5);
	 Hook * Encode2Hook = new Hook((BYTE*)COutPacket_Encode2Address, (BYTE*)&my_encode2, 5);
	 Hook * Encode4Hook = new Hook((BYTE*)COutPacket_Encode4Address, (BYTE*)&my_encode4, 5);
	 Hook * EncodeStringHook = new Hook((BYTE*)COutPacket_EncodeStrAddress, (BYTE*)&my_encodestr, 5);
	 Hook* EncodeBufferHook = new Hook((BYTE*)COutPacket_EncodeBufferAddress, (BYTE*)&my_encodebuffer, 6);

	 Hook * Decode1Hook = new Hook((BYTE*)CInPacket_Decode1Address, (BYTE*)&my_decode1, 7);
	 Hook * Decode2Hook = new Hook((BYTE*)CInPacket_Decode2Address, (BYTE*)&my_decode2, 7);
	 Hook * Decode4Hook = new Hook((BYTE*)CInPacket_Decode4Address, (BYTE*)&my_decode4, 7);
	 Hook * DecodeStringHook = new Hook((BYTE*)CInPacket_DecodeStrAddress, (BYTE*)&my_decodeStr, 7);
	 Hook * DecodeBufferHook = new Hook((BYTE*)CInPacket_DecodeBufferAddress, (BYTE*)&my_decodeBuffer, 6);

	 sendPacketHook->Enable();
	 //recvPacketHook->Enable();
	 CoutPacketHook->Enable();
	 Encode1Hook->Enable();
	 Encode2Hook->Enable();
	 Encode4Hook->Enable();
	 EncodeStringHook->Enable();
	 EncodeBufferHook->Enable();
	 //Decode1Hook->Enable();
	 //Decode2Hook->Enable();
	 //Decode4Hook->Enable();
	 //DecodeStringHook->Enable();
	 //DecodeBufferHook->Enable();
 }