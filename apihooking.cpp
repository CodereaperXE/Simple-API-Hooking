#include <iostream>
#include <Windows.h>//to access winapi
using namespace std;


void* function_addr;//Address where messageboxa lies
void* proxy;//proxy function address
CHAR original[5]={0};//saving the previous bytes of MessageBoxA to restore lated
CHAR patch[5] = { 0 };
DWORD *offset;//for jump instruction the offset between messageboxa address and proxy function address

CHAR message[] = "Hello from Hooked function";//message to be printed by hooked function
CHAR caption[] = "Hooked Function";//message to be printed by hooked function 

int __stdcall hook(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType) { //stdcall immitates MessageBoxA stdcall its also the proxyfunction
	cout << "Hook attached" << endl;
	cout <<"Text is " << (LPCSTR)lpText << endl;
	cout << "Caption is " << (LPCSTR)lpCaption << endl;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)function_addr, original, 5, NULL);//unhooking by writing the original of messageboxa 
	cout << "Unhooking" << endl;
	MessageBoxA(NULL, (LPCSTR)message, (LPCSTR)caption, MB_OK);//calling unhooked messageboxa with custom data
	return 0;
}

int insert_hook() {
	HINSTANCE ins = LoadLibraryA("user32.dll"); //creating instance of user32.dll

	function_addr = GetProcAddress(ins, "MessageBoxA"); //getting messageboxa address
	ReadProcessMemory(GetCurrentProcess(), function_addr, original, 5, NULL);//reading first 5 bytes of messageboxa to restore after unhooking
	proxy = &hook;//proxy function address
	offset = (DWORD*)((DWORD)proxy - ((DWORD)function_addr + 5));//calculating offset for jump instruction

	memcpy(patch, "\xE9", 1);//moving jmp
	memcpy(patch + 1, &offset, 4);//moving offset

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)function_addr, patch, 5, NULL);//writing proxy function offset into address of messageboxa
	
	return 0;
}
int main() {
	
	MessageBoxA(NULL, "Hello", "Hello from function", MB_OK);

	insert_hook();//inserting hook

	MessageBoxA(NULL, "Hello ", "Hijacked", MB_OK);

	MessageBoxA(NULL, "Bye", "Hello from function", MB_OK);
	return 0;
	
}

