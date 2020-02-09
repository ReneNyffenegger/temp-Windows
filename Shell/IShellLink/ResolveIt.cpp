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

   char lpszPath[256];
   // ResolveIt(0, "C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk", lpszPath, 255);

   if (::CoInitializeEx(0, COINIT_MULTITHREADED) != S_OK) {
      std::cout << "CoInitializeEx error" << std::endl;
      return 1;
   }

// HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, char* lpszPath, int iPathBufferSize) { 
    HRESULT hres; 
    IShellLink* iShellLink; 

    TCHAR szGotPath[MAX_PATH]; 
    TCHAR szDescription[MAX_PATH]; 

    WIN32_FIND_DATA wfd; 

 
   *lpszPath = 0; // Assume failure 

 // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
 // has already been called. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&iShellLink); 

    if (! SUCCEEDED(hres)) {

      std::cout << "CoCreateInstance error" << std::endl;
      return 0;

    }


//  { 
    IPersistFile* iPersistFile; 
 
  // Get a pointer to the IPersistFile interface. 
     hres = iShellLink->QueryInterface(IID_IPersistFile, (void**)&iPersistFile); 
        
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
 
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Load the shortcut. 
       hres = iPersistFile->Load(lnkFile, STGM_READ); 

       if (! SUCCEEDED(hres)) { 
             std::cout << "QueryInterface(IID_IPersistFile) error" << std::endl;
             return 0;
       }
            
//          if (SUCCEEDED(hres)) { 
                // Resolve the link. 
        hres = iShellLink->Resolve((HWND) 0, 0); 

        if (! SUCCEEDED(hres)) { 
             std::cout << "..." << std::endl;
             return 0;
        }
       
                    // Get the path to the link target. 
        hres = iShellLink->GetPath(
            szGotPath,
            MAX_PATH,
//         (WIN32_FIND_DATA*)&wfd,
           &wfd,
            SLGP_SHORTPATH); 

         if (! SUCCEEDED(hres)) {
             std::cout << "...";
             return 0;
         }

      // Get the description of the target. 
         hres = iShellLink->GetDescription(szDescription, MAX_PATH); 

        if (! SUCCEEDED(hres)) {
              return 0;
        }
        std::cout << "Description: " << szDescription << std::endl;

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
//                      }
//        }
//              } 
//          } 

            // Release the pointer to the IPersistFile interface. 
            iPersistFile->Release(); 
//      } 

        // Release the pointer to the IShellLink interface. 
        iShellLink->Release(); 

   std::cout << "lpszPath: " << lpszPath << std::endl;

//  } 
//    return hres; 
}





