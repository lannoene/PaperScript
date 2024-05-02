inc "memory.s"

public max_t strlen(char[] s) {
	max_t i = 0;
	while(s[i++] != '\0') {}
	return i - 1;
}

public strcpy(char[] s, const char[] oldS) {
	max_t length = strlen(oldS);

	memcpy(s, oldS, length);
	s[length] = '\0';
}