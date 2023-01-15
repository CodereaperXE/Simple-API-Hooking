#include <iostream>
#include <Windows.h>
using namespace std;


char saved_bytes[4];
void* function_addr;
void* proxy;
CHAR original[5]={0};
CHAR patch[5] = { 0 };
DWORD *offset;

CHAR message[] = "Hello from Hooked function";
CHAR caption[] = "Hooked Function";

int __stdcall hook(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType) {
	cout << "Hook attached" << endl;
	cout <<"Text is " << (LPCSTR)lpText << endl;
	cout << "Caption is " << (LPCSTR)lpCaption << endl;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)function_addr, original, 5, NULL);
	cout << "Unhooking" << endl;
	MessageBoxA(NULL, (LPCSTR)message, (LPCSTR)caption, MB_OK);
	return 0;
}

int insert_hook() {
	HINSTANCE ins = LoadLibraryA("user32.dll");

	function_addr = GetProcAddress(ins, "MessageBoxA");
	ReadProcessMemory(GetCurrentProcess(), function_addr, original, 5, NULL);
	proxy = &hook;
	offset = (DWORD*)((DWORD)proxy - ((DWORD)function_addr + 5));

	memcpy(patch, "\xE9", 1);
	memcpy(patch + 1, &offset, 4);

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)function_addr, patch, 5, NULL);
	
	return 0;
}
int main() {
	
	MessageBoxA(NULL, "Hello", "Hello from function", MB_OK);

	insert_hook();

	MessageBoxA(NULL, "Hello ", "Hijacked", MB_OK);

	MessageBoxA(NULL, "Bye", "Hello from function", MB_OK);
	return 0;
	
}

