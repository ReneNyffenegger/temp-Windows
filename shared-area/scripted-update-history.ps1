$wuSess   = new-object -comObject Microsoft.Update.Session
$wuSearch = $wuSess.createUpdateSearcher()
PS C:\Windows\system32> $wuSearch.search("isInstalled=1").updates | select-object lastDeploymentChangeTime, title

LastDeploymentChangeTime Title
------------------------ -----
5/16/2019 12:00:00 AM    2019-02 Update for Windows 10 Version 1809 for x64-based Systems (KB4465065)
8/13/2019 12:00:00 AM    Microsoft .NET Framework 4.8 Language Packs for Windows 10 Version 1809 for x64 (KB4087642)
2/11/2020 12:00:00 AM    Windows Malicious Software Removal Tool x64 - February 2020 (KB890830)
2/11/2020 12:00:00 AM    2020-02 Cumulative Update for .NET Framework 3.5, 4.7.2 and 4.8 for Windows 10 Version 1809 for x64 (KB4538122)
2/11/2020 12:00:00 AM    2020-02 Security Update for Adobe Flash Player for Windows 10 Version 1809 for x64-based Systems (KB4537759)
3/4/2020 12:00:00 AM     Update for Windows Defender Antivirus antimalware platform - KB4052623 (Version 4.18.2001.10)
2/11/2020 12:00:00 AM    2020-02 Cumulative Update for Windows 10 Version 1809 for x64-based Systems (KB4532691)
2/18/2020 12:00:00 AM    2019-12 Update for Windows 10 Version 1809 for x64-based Systems (KB4023057)
