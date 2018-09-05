#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

/*Aleksandr Valai, gr. 13*/
BOOL GetProcessList();
BOOL ListProcessModules(DWORD dwPID);
BOOL ListProcessThreads(DWORD dwOwnerPID);

int main()
{
	GetProcessList();
	return 0;
}

BOOL GetProcessList()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("Error with creating processes snapshot"));
		return(FALSE);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		_tprintf(TEXT("Error with the first process"));
		CloseHandle(hProcessSnap);
		return(FALSE);
	}


	do
	{
		_tprintf(TEXT("\n\n====================================================="));
		_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
		_tprintf(TEXT("\n-------------------------------------------------------"));

		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
			_tprintf(TEXT("\n	Cannot open process"));
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				_tprintf(TEXT("\n	Cannot get priority class"));
			CloseHandle(hProcess);
		}

		_tprintf(TEXT("\n  Process ID        = %d"), pe32.th32ProcessID);
		_tprintf(TEXT("\n  Thread count      = %d"), pe32.cntThreads);
		_tprintf(TEXT("\n  Parent process ID = %d"), pe32.th32ParentProcessID);
		_tprintf(TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase);
		if (dwPriorityClass)
			_tprintf(TEXT("\n  Priority class    = %d"), dwPriorityClass);
		ListProcessModules(pe32.th32ProcessID);
		ListProcessThreads(pe32.th32ProcessID);

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(TRUE);
}


BOOL ListProcessModules(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("\n Error with CreateToolhelp32Snapshot (of modules)"));
		return(FALSE);
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(hModuleSnap, &me32))
	{
		_tprintf(TEXT("Module32First"));
		CloseHandle(hModuleSnap);
		return(FALSE);
	}
	int i = 0;
	do
	{
		i++;
		_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
		_tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
		_tprintf(TEXT("\n     Process ID     = %d"), me32.th32ProcessID);
		_tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
		_tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
		_tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
		_tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);

	} while (Module32Next(hModuleSnap, &me32));
	_tprintf(TEXT("\n-------------------------------------------------------"));
	_tprintf(TEXT("\n     Modules count = %d"), i);
	_tprintf(TEXT("\n-------------------------------------------------------"));
	CloseHandle(hModuleSnap);

	return(TRUE);
}

BOOL ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		_tprintf(TEXT("Thread32First"));
		CloseHandle(hThreadSnap);
		return(FALSE);
	}

	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			_tprintf(TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
			_tprintf(TEXT("\n     Base priority  = %d"), te32.tpBasePri);
			_tprintf(TEXT("\n     Delta priority = %d"), te32.tpDeltaPri);
			_tprintf(TEXT("\n"));
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(TRUE);
}