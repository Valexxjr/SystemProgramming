/*Alexander Valai, group 13, lab 3*/
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define MSGLEN 150

char SendAdress(DWORD szAdress)
{
	HANDLE hNamedPipe;
	LPCWSTR pipeName = L"\\\\.\\pipe\\pipe_with_str";

	hNamedPipe = CreateFile(pipeName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("Connection with the named pipe failed.\n");
		return 0;
	}
	DWORD dwBytesWritten;
	printf("Wating for process B read data...\n");
	if (!WriteFile(hNamedPipe, &szAdress, sizeof(szAdress), &dwBytesWritten, NULL))
	{
		printf("Writing to the named pipe failed\n");
		CloseHandle(hNamedPipe);
		return 0;
	}
	CloseHandle(hNamedPipe);
	printf("Process B got data.\n");
	return 1;
}


void main()
{
	DWORD messageSize = sizeof(char)*MSGLEN;
	char *a = (char*)malloc(messageSize);
	printf("Enter message: ");
	gets_s(a, MSGLEN - 1);

	DWORD process_B_ID = 0;
	HANDLE snapshot;
	PROCESSENTRY32 processEntry;
	if ((snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
		printf("Can not open processes\n");
	}
	else {
		processEntry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(snapshot, &processEntry);
		do {
			if (wcscmp(processEntry.szExeFile, (LPCWSTR)L"processB.exe") == 0) {
				process_B_ID = processEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &processEntry));
		CloseHandle(snapshot);
	}
	if (!process_B_ID) {
		printf("Can not find process B\n");
	}
	else {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_B_ID);
		LPVOID messageAddress = VirtualAllocEx(hProcess, 0, messageSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(hProcess, messageAddress, a, messageSize, 0);
		SendAdress((DWORD)messageAddress);
		VirtualFreeEx(hProcess, messageAddress, 0, MEM_RELEASE);
	}
	free(a);
	system("pause");
}