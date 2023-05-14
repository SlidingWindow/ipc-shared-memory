#include <stdio.h>
#include <Windows.h>

#define EXIT_SUCCESS 0
#define EXIT_INVALID_ARG_COUNT 1
#define EXIT_SHARED_MEM_OPEN_FAILED 2
#define EXIT_SHARED_MEM_MAP_FAILED 3

int ReadSharedMemory(LPCSTR lpName) {
	HANDLE hFileMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, lpName);
	
	if (hFileMapping == NULL) {
		printf("\n[-]Failed to open shared memory object (%d)", GetLastError());
		return EXIT_SHARED_MEM_OPEN_FAILED;
	}

	//Store the starting address of the mapped view into lpMapStartAddress
	LPVOID lpMapStartAddress = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	
	if (lpMapStartAddress == NULL) {
		printf("Failed to map the view of shared memory (%d)", GetLastError());
		CloseHandle(hFileMapping);
		return EXIT_SHARED_MEM_MAP_FAILED;
	}

	printf("\n[+]Dumping data from shared memory:\n");
	printf("%s\n", (LPCSTR)lpMapStartAddress); // Typecast lpMapStartAddress to LPCSTR
	UnmapViewOfFile(lpMapStartAddress);
	CloseHandle(hFileMapping);

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	LPCSTR hMappingName = NULL; // OpenFIleMappingA() expects the section object name of type LPCSTR

	if (argc != 2) {
		printf("\n[+]Usage: %s SharedMemoryName", argv[0]);
		exit(EXIT_INVALID_ARG_COUNT);
	}

	hMappingName = argv[1];
	ReadSharedMemory(hMappingName);

}