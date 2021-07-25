#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
 
// using namespace std;
 
void CheckForLocks(std::wstring fullPath, std::vector<unsigned long> *processes);
bool CloseHandles(std::wstring fullPath, int process);
