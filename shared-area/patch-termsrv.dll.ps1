$svc_UmRdpService_status = (get-service UmRdpService).status
$svc_TermService_status  = (get-service TermService ).status
# $svc_TermService_status = get-service TermService | get-member    status

write-host "Status of service UmRdpService: $svc_TermService_status"
write-host "Status of service TermService:  $svc_TermService_status"

stop-service UmRdpService
stop-service TermService

$termsrv_dll_acl = get-acl c:\windows\system32\termsrv.dll

$termsrv_dll_owner = $termsrv_dll_acl.owner
write-host "Owner of termsrv.dll:           $termsrv_dll_owner"

copy-item    c:\windows\system32\termsrv.dll c:\windows\system32\termsrv.dll.copy

#
#   Take ownership of the DLL.
#
takeown   /f c:\windows\system32\termsrv.dll

$new_termsrv_dll_owner = (get-acl c:\windows\system32\termsrv.dll).owner

#
#    /g username:F - grant (F)ull control to username:
#
# cacls c:\windows\system32\termsrv.dll /G $new_termsrv_dll_owner:F
cacls c:\windows\system32\termsrv.dll /G rene:F

#
#    https://stackoverflow.com/a/57342311/180275
#
# Read the entire file content as a [byte[]] array.
# Note: Use PowerShell *Core* syntax. 
# In *Windows PowerShell*, replace `-AsByteStream` with `-Encoding Byte`
# `-Raw` ensures that the file is efficiently read as [byte[]] array at once.
# $byteArray = Get-Content C:\OldFile.exe -Raw -AsByteStream
# $dll_as_bytes = get-content c:\windows\system32\termsrv.dll -raw -asByteStream
  $dll_as_bytes = get-content c:\windows\system32\termsrv.dll -raw -encoding byte

# Convert the byte array to a single-line "byte string", 
# where the whitespace-separated tokens are the hex. encoding of a single byte.
# If you want to guaranteed that even byte values < 0x10 are represented as
# *pairs* of hex digits, use 'X2' instead.
# $byteString = $byteArray.ForEach('ToString', 'X') -join ' '
$dll_as_text = $dll_as_bytes.forEach('ToString', 'X2') -join ' '

# Perform the replacement.
# Note that since the string is guaranteed to be single-line, 
# inline option `(?s)` isn't needed.
# Also note how the hex-digit sequences representing bytes are also separated
# by spaces in the search and replacement strings.

                                                               # http://woshub.com/how-to-allow-multiple-rdp-sessions-in-windows-10/
# $dll_as_text_replaced = $dll_as_text -replace '39 81 3C 06 00 00 0F 84 5D 61 01 00', 'B8 00 01 00 00 89 81 38 06 00 00 90' # Windows 1909
  $dll_as_text_replaced = $dll_as_text -replace '39 81 3C 06 00 00 0F 84 5D 61 01 00', 'B8 00 01 00 00 89 81 38 06 00 00 90' # Windows 1903
# $dll_as_text_replaced = $dll_as_text -replace '39 81 3C 06 00 00 0F 84 3B 2B 01 00', 'B8 00 01 00 00 89 81 38 06 00 00 90' # Windows 1809
# $dll_as_text_replaced = $dll_as_text -replace '8B 99 3C 06 00 00 8B B9 38 06 00 00', 'B8 00 01 00 00 89 81 38 06 00 00 90' # Windows 1803
# $dll_as_text_replaced = $dll_as_text -replace '8B 99 3C 06 00 00 8B B9 38 06 00 00', 'B8 00 01 00 00 89 81 38 06 00 00 90' # Windows 1803


# Convert the byte string back to a [byte[]] array, and save it to the
# target file.
# Note how the array is passed as an *argument*, via parameter -Value, 
# rather than via the pipeline, because that is much faster.
# Again, in *Windows PowerShell* use `-Encoding Byte` instead of `-AsByteStream`.
# [byte[]] $newByteArray = -split $byteString -replace '^', '0x'
  [byte[]] $dll_as_bytes_replaced = -split $dll_as_text_replaced -replace '^', '0x'

# Set-Content "C:\NewFile.exe" -AsByteStream -Value $newByteArray
  set-content c:\windows\system32\termsrv.dll.patched -encoding byte -Value $dll_as_bytes_replaced

fc.exe /b c:\windows\system32\termsrv.dll.patched c:\windows\system32\termsrv.dll
#
# Expected output something like:
#
#  0001F215: B8 39
#  0001F216: 00 81
#  0001F217: 01 3C
#  0001F218: 00 06
#  0001F21A: 89 00
#  0001F21B: 81 0F
#  0001F21C: 38 84
#  0001F21D: 06 5D
#  0001F21E: 00 61
#  0001F21F: 00 01
#  0001F220: 90 00


# copy-item c:\windows\system32\termsrv.dll.copy    c:\windows\system32\termsrv.dll
copy-item c:\windows\system32\termsrv.dll.patched c:\windows\system32\termsrv.dll

set-acl c:\windows\system32\termsrv.dll $termsrv_dll_acl

# start-service UmRdpService
# start-service TermService
sc start TermService
sc start UmRdpService

# Can service not be started because of
  - HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Terminal Server
    Value TSEnabled (which should be set to 1?)


# $dll_as_text = get-content c:\windows\system32\termsrv.dll -raw
# $dll_as_text_replaced = $dll_as_text -replace '\x39\x81\x3C\x06\x00\x00\x0F\x84\x5D\x61\x01\x00', '\xB8\x00\x01\x00\x00\x89\x81\x38\x06\x00\x00\x90'
# out-file c:\windows\system32\termsrv.dll.patched -inputObject $dll_as_text_replaced



# stop-s
# 
# 39 81 3C 06 00 00 0F 84 B1 7D 02 00
# # copy-item 
