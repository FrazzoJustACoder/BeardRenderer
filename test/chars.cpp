#include <windows.h>

int main() {
	HANDLE o = GetStdHandle(STD_OUTPUT_HANDLE);
	for(int i = 0; i < 256;) {
		SetConsoleMode(o, 0);
		for(int j = 0; j < 16; j++, i++)
			WriteConsole(o, &i, 1, 0, 0);
		SetConsoleMode(o, ENABLE_PROCESSED_OUTPUT);
		WriteConsole(o, "\n", 1, 0, 0);
	}
	return 0;
}