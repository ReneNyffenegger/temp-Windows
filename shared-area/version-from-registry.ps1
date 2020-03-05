get-itemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion' | select-object `
  currentBuild,
  currentMajorVersionNumber,
  currentMinorVersionNumber,
  releaseId,
  editionId,
  productName

CurrentBuild              : 17763
CurrentMajorVersionNumber : 10
CurrentMinorVersionNumber : 0
ReleaseId                 : 1809
EditionID                 : Professional
ProductName               : Windows 10 Pro
