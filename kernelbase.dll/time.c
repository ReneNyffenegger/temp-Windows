//
//  gcc -nodefaultlibs -nostartfiles -nostdlib .\time.c -luser32 -lkernel32 -lmsvcrt -o time.exe
//

#include <windows.h>

//
// Add declaration for undocumented function
// time.
//

int (__stdcall *time)(int*);

// int __stdcall time(int*);

ULONG __stdcall start (void* PEB) {

   int unixTime_param;

   HMODULE h = LoadLibraryA("kernelbase.dll");
   time=GetProcAddress(h, "time");



   char buf[100];
   DWORD charsWritten;
   HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);

   wsprintfA(buf, "time = %d", h);
   WriteConsoleA(stdOut, buf, lstrlen(buf), &charsWritten, NULL);

   int unixTime_return = time(&unixTime_param);

   wsprintfA(buf, "seconds since 1970 (parameter   ): %d\n", unixTime_param );
   WriteConsoleA(stdOut, buf, lstrlen(buf), &charsWritten, NULL);

   wsprintfA(buf, "seconds since 1970 (return value): %d\n", unixTime_return);
   WriteConsoleA(stdOut, buf, lstrlen(buf), &charsWritten, NULL);

   return 0;
}
