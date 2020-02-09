//   
//    cl /nologo /EHsc ResolveIt.cpp ole32.lib
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

   char lpszPath[256];
   // ResolveIt(0, "C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk", lpszPath, 255);

   if (::CoInitializeEx(0, COINIT_MULTITHREADED) != S_OK) {
      std::cout << "CoInitializeEx error" << std::endl;
      return 1;
   }

// HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, char* lpszPath, int iPathBufferSize) { 
    HRESULT hres; 
    IShellLink* psl; 
    TCHAR szGotPath[MAX_PATH]; 
    TCHAR szDescription[MAX_PATH]; 
    WIN32_FIND_DATA wfd; 
 
   *lpszPath = 0; // Assume failure 

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
    if (! SUCCEEDED(hres)) {

      std::cout << "CoCreateInstance error" << std::endl;
      return 0;

    }


//  { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
        
        if (SUCCEEDED(hres)) { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, 
               "C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk",
//          lpszLinkFile,
               -1, wsz, MAX_PATH); 
 
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Load the shortcut. 
            hres = ppf->Load(wsz, STGM_READ); 
            
            if (SUCCEEDED(hres)) { 
                // Resolve the link. 
                hres = psl->Resolve((HWND) 0, 0); 

                if (SUCCEEDED(hres)) { 
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH); 

                    if (SUCCEEDED(hres)) { 
                        // Get the description of the target. 
                        hres = psl->GetDescription(szDescription, MAX_PATH); 

                        if (SUCCEEDED(hres)) {
                            hres = StringCbCopy((STRSAFE_LPSTR) lpszPath, 
                               255, // iPathBufferSize
                               szGotPath
                            );

                            if (SUCCEEDED(hres))
                            {
                                // Handle success
                            }
                            else
                            {
                                // Handle the error
                            }
                        }
                    }
                } 
            } 

            // Release the pointer to the IPersistFile interface. 
            ppf->Release(); 
        } 

        // Release the pointer to the IShellLink interface. 
        psl->Release(); 

   std::cout << "lpszPath: " << lpszPath << std::endl;

//  } 
//    return hres; 
}





