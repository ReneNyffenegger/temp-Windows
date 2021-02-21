//
//  gcc -Wall -Wextra -Wno-unused-parameter -nodefaultlibs -nostartfiles -nostdlib .\time.c -luser32 -lkernel32 '-Wl,-estart' -o time.exe
//

#include <windows.h>

HANDLE stdOut;

void print(const char* txt) {
   DWORD charsWritten;
   WriteConsoleA(stdOut, txt, lstrlen(txt), &charsWritten, NULL);
}

//
// Add typedef for for undocumented function
// time in kernelbase.dll:
//
typedef long long int (WINAPI *f_time)(long long int*);

ULONG WINAPI start (void* PEB) {

   stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   if (stdOut == INVALID_HANDLE_VALUE) {
      return 1;
   }

   HMODULE kernelBase = LoadLibraryA("kernelbase.dll");

   if (!kernelBase) {
      print("Could not load library kernelbase.dll\n");
      return 2;
   }

   f_time time = ((f_time) GetProcAddress(kernelBase, "time"));

   if (!time) {
      print("Could not get address of time\n");
      return 2;
   }

   char buf[100];

   long long int unixTime_param;
   long long int unixTime_return = time(&unixTime_param);

   wsprintfA(buf, "Seconds since 1970 (parameter   ): %I64d\n", unixTime_param );
   print(buf);

   wsprintfA(buf, "Seconds since 1970 (return value): %I64d\n", unixTime_return);
   print(buf);

   return 0;
}
