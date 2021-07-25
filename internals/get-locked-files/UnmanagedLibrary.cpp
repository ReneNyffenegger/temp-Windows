#include "UnmanagedLibrary.h"
 
#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2
 
typedef NTSTATUS(NTAPI *_NtQuerySystemInformation)(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI *_NtDuplicateObject)(HANDLE SourceProcessHandle, HANDLE SourceHandle, HANDLE TargetProcessHandle, PHANDLE TargetHandle, ACCESS_MASK DesiredAccess, ULONG Attributes, ULONG Options);
typedef NTSTATUS(NTAPI *_NtQueryObject)(HANDLE ObjectHandle, ULONG ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);
 
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
 
typedef struct _SYSTEM_HANDLE {
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;
 
typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
 
typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS
} POOL_TYPE, *PPOOL_TYPE;
 
typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING Name;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccess;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    USHORT MaintainTypeList;
    POOL_TYPE PoolType;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;
 
using namespace std;
 
PVOID GetLibraryProcAddress(PSTR LibraryName, PSTR ProcName) {
    return GetProcAddress(GetModuleHandleA(LibraryName), ProcName);
}
 
_NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetLibraryProcAddress("ntdll.dll", "NtQuerySystemInformation");
_NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetLibraryProcAddress("ntdll.dll", "NtDuplicateObject");
_NtQueryObject NtQueryObject = (_NtQueryObject)GetLibraryProcAddress("ntdll.dll", "NtQueryObject");
 
struct QueryStructure {
    HANDLE dupHandle;
    PVOID objectNameInfo;
    ULONG objectInfoLength;
    ULONG returnLength;
    NTSTATUS result;
};
 
HANDLE beginQuery = CreateEvent(0, FALSE, FALSE, 0);
HANDLE endQuery = CreateEvent(0, FALSE, FALSE, 0);
QueryStructure queryStructure;
 
HANDLE beginQueryCloseHandle = CreateEvent(0, FALSE, FALSE, 0);
HANDLE endQueryCloseHandle = CreateEvent(0, FALSE, FALSE, 0);
QueryStructure queryStructureCloseHandle;
 
DWORD WINAPI queryThread(LPVOID parameter) {
    while (WaitForSingleObject(beginQuery, INFINITE) == WAIT_OBJECT_0)
    {
        queryStructure.result = NtQueryObject(queryStructure.dupHandle, ObjectNameInformation, queryStructure.objectNameInfo, queryStructure.objectInfoLength, &queryStructure.returnLength);
        SetEvent(endQuery);
    }
    return 0;
}
 
DWORD WINAPI queryThreadCloseHandle(LPVOID parameter)
{
    while (WaitForSingleObject(beginQueryCloseHandle, INFINITE) == WAIT_OBJECT_0)
    {
        queryStructureCloseHandle.result = NtQueryObject(queryStructureCloseHandle.dupHandle, ObjectNameInformation, queryStructureCloseHandle.objectNameInfo, queryStructureCloseHandle.objectInfoLength, &queryStructureCloseHandle.returnLength);
        SetEvent(endQueryCloseHandle);
    }
    return 0;
}
 
HANDLE queryThreadHandle = CreateThread(0, 0, &queryThread, 0, 0, 0);
HANDLE queryThreadCloseHandleHandle = CreateThread(0, 0, &queryThreadCloseHandle, 0, 0, 0);
 
void ConvertPath(wstring *path, map<wstring, wstring> *volumes) {

    for (map<wstring, wstring>::iterator i = (*volumes).begin(); i != (*volumes).end(); ++i) {

        if ((*path).compare(0, (*i).first.size(), (*i).first) == 0) {
            *path = (*path).replace(0, (*i).first.size(), (*i).second);
            break;
        }
    }
}
 
void CheckForLocks(wstring fullPath, vector<unsigned long> *processes) {
    DWORD   CharCount = 0;
    WCHAR   DeviceName[MAX_PATH] = L"";
    HANDLE  FindHandle = INVALID_HANDLE_VALUE;
    size_t  Index = 0;
    BOOL    Success = FALSE;
    WCHAR   VolumeName[MAX_PATH] = L"";
    map<wstring, wstring> volumes;
    FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

    while (true) {
        Index = wcslen(VolumeName) - 1;
        if (VolumeName[0] != L'\\' || VolumeName[1] != L'\\' || VolumeName[2] != L'?' || VolumeName[3] != L'\\' || VolumeName[Index] != L'\\')
        {
            break;
        }
        VolumeName[Index] = L'\0';
        CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
        VolumeName[Index] = L'\\';
        if (CharCount == 0)
        {
            break;
        }
        DWORD   size = MAX_PATH + 1;
        PWCHAR  name = NULL;
        BOOL    success = FALSE;
        while (!success)
        {
            name = (PWCHAR) new BYTE[size * sizeof(WCHAR)];
            success = GetVolumePathNamesForVolumeNameW(VolumeName, name, size, &size);
            if (!success)
            {
                delete[] name;
                name = NULL;
            }
        }
        volumes[DeviceName + wstring(L"\\")] = name;
        if (name != NULL)
        {
            delete[] name;
            name = NULL;
        }
        Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));
        if (!Success)
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                break;
            }
            break;
        }
    }
    FindVolumeClose(FindHandle);
    FindHandle = INVALID_HANDLE_VALUE;
    NTSTATUS status;
    PSYSTEM_HANDLE_INFORMATION handleInfo;
    ULONG handleInfoSize = 0x10000;
    HANDLE processHandle;
    ULONG i;
    DWORD pid;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process))
    {
        do
        {
            pid = process.th32ProcessID;
            if (!(processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid)))
            {
                continue;
            }
            handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
            while ((status = NtQuerySystemInformation(SystemHandleInformation, handleInfo, handleInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
            {
                handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize *= 2);
            }
            if (!NT_SUCCESS(status))
            {
                return;
            }

            for (i = 0; i < handleInfo->HandleCount; i++) {

                SYSTEM_HANDLE handle = handleInfo->Handles[i];
                HANDLE dupHandle = NULL;
                POBJECT_TYPE_INFORMATION objectTypeInfo;
                PVOID objectNameInfo;
                UNICODE_STRING objectName;
                ULONG returnLength = 0;
                if (handle.ProcessId != pid)
                    continue;
                if (!NT_SUCCESS(NtDuplicateObject(processHandle, (HANDLE)handle.Handle, GetCurrentProcess(), &dupHandle, 0, 0, 0)))
                {
                    continue;
                }
                objectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x1000);
                size_t currentSize = 0x1000;
                objectNameInfo = malloc(currentSize);
                queryStructure.dupHandle = dupHandle;
                queryStructure.objectNameInfo = objectNameInfo;
                queryStructure.objectInfoLength = 0x1000;
                queryStructure.returnLength = returnLength;
                queryStructure.result = -1;
                SetEvent(beginQuery);
                if (WaitForSingleObject(endQuery, 100) == WAIT_TIMEOUT)
                {
                    TerminateThread(queryThreadHandle, 1);
                    CloseHandle(queryThreadHandle);
                    queryThreadHandle = CreateThread(0, 0, &queryThread, 0, 0, 0);
                    CloseHandle(dupHandle);
                    continue;
                }

                if (!NT_SUCCESS(queryStructure.result)) {
                    objectNameInfo = realloc(objectNameInfo, currentSize *= 2);
                    if (!NT_SUCCESS(NtQueryObject(dupHandle, ObjectNameInformation, objectNameInfo, returnLength, NULL)))
                    {
                        free(objectTypeInfo);
                        free(objectNameInfo);
                        CloseHandle(dupHandle);
                        continue;
                    }
                }
                objectName = *(PUNICODE_STRING)objectNameInfo;

                if (objectName.Length) {

                    wstring objectNameAsWString = wstring(objectName.Buffer, objectName.Length / sizeof(WCHAR));
                    ConvertPath(&objectNameAsWString, &volumes);

                    if ((int)objectNameAsWString.find(fullPath) >= 0) {
                        (*processes).push_back(pid);
                        free(objectTypeInfo);
                        free(objectNameInfo);
                        CloseHandle(dupHandle);
                        break;
                    }
                }
                free(objectTypeInfo);
                free(objectNameInfo);
                CloseHandle(dupHandle);
            }
            free(handleInfo);
            CloseHandle(processHandle);
        } while (Process32Next(snapshot, &process));
    }
}
 
bool CloseHandles(wstring fullPath, int process) {

    DWORD   CharCount = 0;
    WCHAR   DeviceName[MAX_PATH] = L"";
    HANDLE  FindHandle = INVALID_HANDLE_VALUE;
    size_t  Index = 0;
    BOOL    Success = FALSE;
    WCHAR   VolumeName[MAX_PATH] = L"";
    map<wstring, wstring> volumes;
    FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));
    while (true)
    {
        Index = wcslen(VolumeName) - 1;
        if (VolumeName[0] != L'\\' || VolumeName[1] != L'\\' || VolumeName[2] != L'?' || VolumeName[3] != L'\\' || VolumeName[Index] != L'\\')
        {
            break;
        }
        VolumeName[Index] = L'\0';
        CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
        VolumeName[Index] = L'\\';
        if (CharCount == 0)
        {
            break;
        }
        DWORD   size = MAX_PATH + 1;
        PWCHAR  name = NULL;
        BOOL    success = FALSE;
        while (!success)
        {
            name = (PWCHAR) new BYTE[size * sizeof(WCHAR)];
            success = GetVolumePathNamesForVolumeNameW(VolumeName, name, size, &size);
            if (!success)
            {
                delete[] name;
                name = NULL;
            }
        }
        volumes[DeviceName + wstring(L"\\")] = name;
        if (name != NULL)
        {
            delete[] name;
            name = NULL;
        }
        Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));
        if (!Success)
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
            {
                break;
            }
            break;
        }
    }
    FindVolumeClose(FindHandle);
    FindHandle = INVALID_HANDLE_VALUE;
    _NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetLibraryProcAddress("ntdll.dll", "NtQuerySystemInformation");
    _NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetLibraryProcAddress("ntdll.dll", "NtDuplicateObject");
    _NtQueryObject NtQueryObject = (_NtQueryObject)GetLibraryProcAddress("ntdll.dll", "NtQueryObject");
    NTSTATUS status;
    PSYSTEM_HANDLE_INFORMATION handleInfo;
    ULONG handleInfoSize = 0x10000;
    HANDLE processHandle;
    ULONG i;
    if (!(processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, process)))
    {
        return false;
    }
    handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
    while ((status = NtQuerySystemInformation(SystemHandleInformation, handleInfo, handleInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
    {
        handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize *= 2);
    }
    if (!NT_SUCCESS(status))
    {
        return false;
    }
    for (i = 0; i < handleInfo->HandleCount; i++)
    {
        SYSTEM_HANDLE handle = handleInfo->Handles[i];
        HANDLE dupHandle = NULL;
        POBJECT_TYPE_INFORMATION objectTypeInfo;
        PVOID objectNameInfo;
        UNICODE_STRING objectName;
        ULONG returnLength = 0;
        if (handle.ProcessId != process)
            continue;
        if (!NT_SUCCESS(NtDuplicateObject(processHandle, (HANDLE)handle.Handle, GetCurrentProcess(), &dupHandle, 0, 0, 0)))
        {
            continue;
        }
        objectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x1000);
        size_t currentSize = 0x1000;
        objectNameInfo = malloc(currentSize);
        queryStructureCloseHandle.dupHandle = dupHandle;
        queryStructureCloseHandle.objectNameInfo = objectNameInfo;
        queryStructureCloseHandle.objectInfoLength = 0x1000;
        queryStructureCloseHandle.returnLength = returnLength;
        queryStructureCloseHandle.result = -1;
        SetEvent(beginQueryCloseHandle);
        if (WaitForSingleObject(endQueryCloseHandle, 100) == WAIT_TIMEOUT)
        {
            TerminateThread(queryThreadCloseHandleHandle, 1);
            CloseHandle(queryThreadCloseHandleHandle);
            queryThreadCloseHandleHandle = CreateThread(0, 0, &queryThreadCloseHandle, 0, 0, 0);
            CloseHandle(dupHandle);
            continue;
        }
        if (!NT_SUCCESS(queryStructureCloseHandle.result))
        {
            objectNameInfo = realloc(objectNameInfo, currentSize *= 2);
            if (!NT_SUCCESS(NtQueryObject(dupHandle, ObjectNameInformation, objectNameInfo, returnLength, NULL)))
            {
                free(objectTypeInfo);
                free(objectNameInfo);
                CloseHandle(dupHandle);
                continue;
            }
        }
        objectName = *(PUNICODE_STRING)objectNameInfo;
        if (objectName.Length)
        {
            wstring objectNameAsWString = wstring(objectName.Buffer, objectName.Length / sizeof(WCHAR));
            ConvertPath(&objectNameAsWString, &volumes);
            if ((int)objectNameAsWString.find(fullPath) >= 0)
            {
                NtDuplicateObject(processHandle, (HANDLE)handle.Handle, 0, 0, 0, 0, 1);
            }
        }
        free(objectTypeInfo);
        free(objectNameInfo);
        CloseHandle(dupHandle);
    }
    free(handleInfo);
    CloseHandle(processHandle);
    return true;
}
