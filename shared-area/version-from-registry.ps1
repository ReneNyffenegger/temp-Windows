get-itemProperty 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion' | select-object `
  currentBuild,
  currentMajorVersionNumber,
  curretnMinorVersionNumber,
  releaseId,
  productName
