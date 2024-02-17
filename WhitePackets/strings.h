unsigned char pLoad [12] = {0x1b, 0x11, 0x25, 0x1c, 0x1c, 0x15, 0x34, 0x19, 0x9, 0x15, 0x22, 0x1b};
void DecryptXOR(char* encrypted_data, size_t data_length, char* key, size_t key_length);
char eKey [] = "MxWhitXXey";
unsigned int pLoadLen = 12;
unsigned int eKeyLen = 10;