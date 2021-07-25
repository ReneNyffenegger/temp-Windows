//
//   cl /EHsc .\UnmanagedLibrary.cpp .\main.cpp
//
#include <iostream>

#include "UnmanagedLibrary.h"

int main() {

   std::vector<unsigned long> processes;
// processes.push_back(GetCurrentProcessId());

   std::wstring path = L"C:\\Users\\Rene\\AppData\\Local\\Temp\\test.db";

   CheckForLocks(path, &processes);


   for (auto process = processes.begin(); process<processes.end(); process++) {
      std::cout << *process << std::endl;

      CloseHandles(path, *process);
   }


}
