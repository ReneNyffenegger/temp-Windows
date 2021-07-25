#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
 
using namespace std;
 
void CheckForLocks(wstring fullPath, vector<unsigned long> *processes);
bool CloseHandles(wstring fullPath, int process);
