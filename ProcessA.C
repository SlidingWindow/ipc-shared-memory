#include <stdio.h>
#include <Windows.h>
#include <aclapi.h>


int main() {
	HANDLE hFileMapping;
	PWSTR SharedMemData;
	LPVOID lpFileMap = NULL; // Initialise to NULL to avoid uninitialised pointer variable error.
	int fMapSize = 4 << 10;
	const wchar_t* fMapName = L"Global\\SharedMemTest";
	char secretData[] = "{\"ConnectionString\":\"0daefa329873bcdefafeae8d5a4d6f29875affdedefaaffededffdfedf\"}";
	
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SID_IDENTIFIER_AUTHORITY sidAuth = SECURITY_NT_AUTHORITY;
	PSID pSidUsersGroup = NULL;
	
	AllocateAndInitializeSid(&sidAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &pSidUsersGroup);
	//AllocateAndInitializeSid(&sidAuth, 3, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, SECURITY_LOCAL_LOGON_RID, 0, 0, 0, 0, 0, &pSidEveryone);


	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = FILE_MAP_ALL_ACCESS; // Full access to all users
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea.Trustee.ptstrName = (LPSTR)pSidUsersGroup;

	PACL pDacl = NULL;
	DWORD dwErr = SetEntriesInAcl(1, &ea, NULL, &pDacl);
	if (dwErr != ERROR_SUCCESS) {
		// to do: error handling
	}

	SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE);

	//Set Security Attributes
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;


	hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, fMapSize, fMapName);

	if (!hFileMapping) {
		printf("[+]Failed to create file mapping for %ls", fMapName);

	}

	lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, fMapSize);

	if (!lpFileMap) {
		printf("[+]Map view of file for %ls failed", fMapName);
	}

	wcscpy_s((PWSTR)lpFileMap, sizeof(secretData), secretData);
		
	SharedMemData = lpFileMap;
	printf("The following connection string was written to the shared memory %ls successfully:\n\n%s", fMapName, SharedMemData);

	
	UnmapViewOfFile(lpFileMap); // Unmap the view
	FreeSid(pSidUsersGroup); //Free the memory since the access permission is set and memory allocated for the SID is no longer needed
	system("PAUSE > nul");

	return 0;

}