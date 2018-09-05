/*Alexander Valai, group 13, lab 3*/
#include <windows.h> 
#include <stdio.h>
int main() {
	LPCWSTR pipeName = L"\\\\.\\pipe\\pipe_with_str";
	HANDLE hNamedPipe;
	hNamedPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
		| PIPE_WAIT, 1, sizeof(DWORD), sizeof(DWORD), 1, NULL);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("Can not create named pipe\n");
		return 1;
	}
	printf("Wating for Process A write data\n");
	ConnectNamedPipe(hNamedPipe, NULL);
	DWORD address;
	DWORD dwBytesRead;
	ReadFile(hNamedPipe, &address, sizeof(address), &dwBytesRead, NULL);
	printf("String get: %s\n", (char*)address);
	CloseHandle(hNamedPipe);
	system("pause");
	return 0;
}