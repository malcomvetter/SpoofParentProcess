#include <Windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <string>


int main(int argc, char **canttrustthis)
{
	std::string targetProcess = "chrome.exe";
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	HANDLE parent;

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (strcmp(entry.szExeFile, targetProcess.c_str()) == 0)
			{
				parent = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				break;
			}
		}
	}

	CloseHandle(snapshot);


	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOEXA si = { 0 };
	SIZE_T sizeToAllocate;

	// Initialize the process start attributes
	InitializeProcThreadAttributeList(NULL, 1, 0, &sizeToAllocate);
	si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, sizeToAllocate);
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &sizeToAllocate);
	
	// Set the parent process
	if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, 
		PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parent, 
		sizeof(HANDLE), NULL, NULL))
	{
		return 0;
	}
	si.StartupInfo.cb = sizeof(STARTUPINFOEXA);


	BOOL success = CreateProcessA(
		NULL,
		const_cast<char *>("C:\\Windows\\system32\\notepad.exe"),
		NULL,
		NULL,
		true,
		EXTENDED_STARTUPINFO_PRESENT,
		NULL,
		NULL,
		(LPSTARTUPINFOA)&si,
		&pi
	);

	CloseHandle(parent);

	return 0;
}