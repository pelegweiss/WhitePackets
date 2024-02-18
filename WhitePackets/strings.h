char eKey[] = "123456789ABC";
size_t eKeyLen = 12;
//VirtualAlloc encrypted
unsigned char vAllocEncrypted [12] = { 0x67, 0x5b, 0x41, 0x40, 0x40, 0x57, 0x5b, 0x79, 0x55, 0x2d, 0x2d, 0x20 };
size_t vAllocEncryptedLen = 12;

void DecryptXOR(char* encrypted_data, size_t data_length, char* key, size_t key_length);