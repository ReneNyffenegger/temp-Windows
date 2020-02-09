//
//  cl /nologo /EHsc ResolveIt.cpp ole32.lib
//  g++ ResolveIt.cpp -lole32
//
//  Very useful was:  https://docs.microsoft.com/en-us/windows/win32/shell/links
//

#include "windows.h"
#include "shobjidl.h"
#include "shlguid.h"
#include "strsafe.h"

#include "shlobj_core.h"
// #include "Shobjidl.h"

#include <iostream>
#include <iomanip>

void printCoord(
        std::string const& txt,
        COORD       coord
     ) {

   std::cout << txt << ": " << coord.X << "x" << coord.Y << std::endl;
}

void printColor(
        std::string const& txt,
        LPCOLORREF  colors,
        int         index
) {

   std::cout << txt <<
   std::dec << std::setfill(' ') << std::setw(2)<< index  << " = " <<
   std::setfill('0') << std::hex << std::setw(2) << (   colors[index]        & 255 ) <<
   std::setfill('0') << std::hex << std::setw(2) << ( ( colors[index] >>  8) & 255 ) <<
   std::setfill('0') << std::hex << std::setw(2) << ( ( colors[index] >> 16) & 255 ) <<
   std::endl;
}

int main() {

   if (::CoInitializeEx(0, COINIT_MULTITHREADED) != S_OK) {
      std::cout << "CoInitializeEx error" << std::endl;
      return 1;
   }

   HRESULT rc;

   IShellLink *iShellLink;
   rc = CoCreateInstance(
        CLSID_ShellLink,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
       (LPVOID*) &iShellLink
   );

   if (! SUCCEEDED(rc)) {
     std::cout << "CoCreateInstance error" << std::endl;
     return 0;
   }


   IPersistFile* iPersistFile;

   rc = iShellLink->QueryInterface(IID_IPersistFile, (LPVOID*) &iPersistFile);

   if (! SUCCEEDED(rc)) {
         std::cout << "QueryInterface(IID_IPersistFile) error" << std::endl;
         return 0;
   }

   wchar_t lnkFile[] = L"C:\\Users\\OMIS.Rene\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\System Tools\\Command Prompt.lnk";

// Load the shortcut.
   rc = iPersistFile->Load(lnkFile, STGM_READ);

   if (! SUCCEEDED(rc)) {
        std::cout << "iPersistFile->Load() error" << std::endl;
        return 0;
   }

   rc = iShellLink->Resolve((HWND) 0, 0);

   if (! SUCCEEDED(rc)) {
        std::cout << "..." << std::endl;
        return 0;
   }


   std::string linkTarget(MAX_PATH, '\x00');
   rc = iShellLink->GetPath(
      &linkTarget[0]  ,
       MAX_PATH       ,
       0              , //  WIN32_FIND_DATA*
       SLGP_SHORTPATH
   );

   if (! SUCCEEDED(rc)) {
       std::cout << "...";
       return 0;
   }

   std::string description(MAX_PATH, '\x00');
   rc = iShellLink->GetDescription(&description[0], MAX_PATH);

   if (! SUCCEEDED(rc)) {
        return 0;
   }

   std::cout << "Description: " << description << std::endl;
   std::cout << "link Target: " << linkTarget  << std::endl;


   WORD hotKey;
   iShellLink -> GetHotkey(&hotKey);
   std::cout << "HotKey:      " << hotKey << std::endl;


   IShellLinkDataList *iShellLinkDataList;
   rc = iShellLink->QueryInterface(IID_IShellLinkDataList, (LPVOID*) &iShellLinkDataList);
   if (! SUCCEEDED(rc)) {
         std::cout << "QueryInterface(IID_IShellLinkDataList) error" << std::endl;
         return 0;
   }

   DWORD flags;
   iShellLinkDataList->GetFlags(&flags);
   #define hasFlag(flagName) \
   if (flags & flagName) { \
      std::cout << #flagName ": yes" << std::endl; \
   } \
   else { \
      std::cout << #flagName ": no" << std::endl; \
   }

   hasFlag(SLDF_HAS_LINK_INFO   )
   hasFlag(SLDF_HAS_RELPATH     )
   hasFlag(SLDF_HAS_WORKINGDIR  )
   hasFlag(SLDF_HAS_ARGS        )
   hasFlag(SLDF_HAS_ICONLOCATION)
   hasFlag(SLDF_UNICODE         )
   hasFlag(SLDF_RUNAS_USER      )
   hasFlag(SLDF_FORCE_UNCNAME   )

   LPNT_CONSOLE_PROPS pNT_CONSOLE_PROPS;
   iShellLinkDataList->CopyDataBlock(NT_CONSOLE_PROPS_SIG, (LPVOID*) &pNT_CONSOLE_PROPS);

   printCoord("Window size  ", pNT_CONSOLE_PROPS->dwWindowSize  );
   printCoord("Font size    ", pNT_CONSOLE_PROPS->dwFontSize    );
   printCoord("Window origin", pNT_CONSOLE_PROPS->dwWindowOrigin);
   std::cout  << "Auto pos:  : " << pNT_CONSOLE_PROPS->bInsertMode << std::endl;

   std::cout << "History buf size:    " << pNT_CONSOLE_PROPS->uHistoryBufferSize      << std::endl;
   std::cout << "nof History buffers: " << pNT_CONSOLE_PROPS->uNumberOfHistoryBuffers << std::endl;
   std::cout << "Input buffer size:   " << pNT_CONSOLE_PROPS->nInputBufferSize        << std::endl;

   std::wcout << "Font Face:   " << pNT_CONSOLE_PROPS->FaceName    << std::endl; // Note: FaceName is a wchar_t, hence the wcout
   std::cout  << "Font family: " << pNT_CONSOLE_PROPS->uFontFamily << std::endl;
   std::cout  << "Font weight: " << pNT_CONSOLE_PROPS->uFontWeight << std::endl;
   std::cout  << "Cursor size: " << pNT_CONSOLE_PROPS->uCursorSize << std::endl;
   std::cout  << "Full screen: " << pNT_CONSOLE_PROPS->bFullScreen << std::endl;
   std::cout  << "Quick Edit:  " << pNT_CONSOLE_PROPS->bQuickEdit  << std::endl;
   std::cout  << "Insert Mode: " << pNT_CONSOLE_PROPS->bInsertMode << std::endl;

   int indexColorFG, indexColorBG;
   indexColorFG =   ( pNT_CONSOLE_PROPS->wFillAttribute )        & 15;
   indexColorBG = ( ( pNT_CONSOLE_PROPS->wFillAttribute ) >> 4 ) & 15;

   LPCOLORREF colors = pNT_CONSOLE_PROPS->ColorTable;

   printColor("Foreground color", colors, indexColorFG);
   printColor("Background color", colors, indexColorBG);

   iPersistFile->Release();
   iShellLink->Release();
}
