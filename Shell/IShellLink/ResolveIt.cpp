//   
//    cl /nologo /EHsc ResolveIt.cpp ole32.lib
//    g++ ResolveIt.cpp -lole32
// 
//    https://docs.microsoft.com/en-us/windows/win32/shell/links
//    ----------------------------------------------------------
//
// ResolveIt - Uses the Shell's IShellLink and IPersistFile interfaces 
//             to retrieve the path and description from an existing shortcut. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// hwnd         - A handle to the parent window. The Shell uses this window to 
//                display a dialog box if it needs to prompt the user for more 
//                information while resolving the link.
// lpszLinkFile - Address of a buffer that contains the path of the link,
//                including the file name.
// lpszPath     - Address of a buffer that receives the path of the link
//                target, including the file name.
// lpszDesc     - Address of a buffer that receives the description of the 
//                Shell link, stored in the Comment field of the link
//                properties.

// #include "stdafx.h"
#include "windows.h"
#include "shobjidl.h"
#include "shlguid.h"
#include "strsafe.h"

#include <iostream>
                            
int main() {

// char lpszPath[256];
   // ResolveIt(0, "C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk", lpszPath, 255);

   if (::CoInitializeEx(0, COINIT_MULTITHREADED) != S_OK) {
      std::cout << "CoInitializeEx error" << std::endl;
      return 1;
   }

// HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, char* lpszPath, int iPathBufferSize) { 
    HRESULT     hres; 

//  TCHAR szGotPath[MAX_PATH]; 

//  WIN32_FIND_DATA wfd; 

 
// *lpszPath = 0; // Assume failure 

 // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
 // has already been called. 

    IShellLink *iShellLink; 
    hres = CoCreateInstance(
         CLSID_ShellLink,
         NULL,
         CLSCTX_INPROC_SERVER,
         IID_IShellLink,
        (LPVOID*) &iShellLink
    ); 

    if (! SUCCEEDED(hres)) {
      std::cout << "CoCreateInstance error" << std::endl;
      return 0;
    }


//  { 
    IPersistFile* iPersistFile; 
 
  // Get a pointer to the IPersistFile interface. 
     hres = iShellLink->QueryInterface(IID_IPersistFile, (LPVOID*) &iPersistFile); 
        
     if (! SUCCEEDED(hres)) { 
           std::cout << "QueryInterface(IID_IPersistFile) error" << std::endl;
           return 0;
     }
       
//    WCHAR lnkFile[MAX_PATH]; 
 
      wchar_t lnkFile[] = L"C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk";

//X    // Ensure that the string is Unicode. 
//X       MultiByteToWideChar(
//X             CP_ACP,
//X             0, 
//X            "C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk",
//X //          lpszLinkFile,
//X            -1,
//X             lnkFile,
//X             MAX_PATH
//X      ); 
 
    // Load the shortcut. 
       hres = iPersistFile->Load(lnkFile, STGM_READ); 

       if (! SUCCEEDED(hres)) { 
             std::cout << "iPersistFile->Load() error" << std::endl;
             return 0;
       }
            
        hres = iShellLink->Resolve((HWND) 0, 0); 

        if (! SUCCEEDED(hres)) { 
             std::cout << "..." << std::endl;
             return 0;
        }
       
     // Get the path to the link target. 
         
        std::string linkTarget(MAX_PATH, '\x00');
        hres = iShellLink->GetPath(
           &linkTarget[0]  ,
            MAX_PATH       ,
            0              , //  WIN32_FIND_DATA*
            SLGP_SHORTPATH
        ); 

        if (! SUCCEEDED(hres)) {
            std::cout << "...";
            return 0;
        }

        std::string description(MAX_PATH, '\x00');
        hres = iShellLink->GetDescription(&description[0], MAX_PATH); 

        if (! SUCCEEDED(hres)) {
             return 0;
        }

        std::cout << "Description: " << description << std::endl;
        std::cout << "link Target: " << linkTarget  << std::endl;


        WORD hotKey;
        iShellLink -> GetHotkey(&hotKey);
        std::cout << "HotKey:      " << hotKey << std::endl;

        iPersistFile->Release(); 
        iShellLink->Release(); 
}
