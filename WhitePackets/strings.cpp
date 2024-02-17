void DecryptXOR(char* encrypted_data, size_t data_length, char* key, size_t key_length) {
	int key_index = 0;

	for (int i = 0; i < data_length; i++) {
		if (key_index == key_length - 1) key_index = 0;

		encrypted_data[i] = encrypted_data[i] ^ key[key_index];
		key_index++;
	}
}