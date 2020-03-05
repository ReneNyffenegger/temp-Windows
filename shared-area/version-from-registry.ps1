get-itemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion' | select-object `
  currentBuild,
  currentMajorVersionNumber,
  curretnMinorVersionNumber,
  releaseId,
  productName

CurrentBuild              : 17763
CurrentMajorVersionNumber : 10
curretnMinorVersionNumber :
ReleaseId                 : 1809
ProductName               : Windows 10 Pro
