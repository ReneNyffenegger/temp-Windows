#
#  https://github.com/csteelatgburg/scripts/blob/master/Windows/GetSleepAndHibernateValues.ps1
#

$plan = Get-WmiObject -Class win32_powerplan -Namespace root\cimv2\power | Where-Object {$_.IsActive -eq "True"}
$regex = [regex]“{(.*?)}$” 
$planGuid = $regex.Match($plan.instanceID.Tostring()).groups[1].value 
$power = powercfg -query $planGuid 238c9fa8-0aad-41ed-83f4-97be242c8f20
$sleep = powercfg -query $planGuid SUB_SLEEP STANDBYIDLE
$sleepValue = $sleep  | Select-String "Current AC Power Setting Index"
$hibernate = powercfg -query $planGuid SUB_SLEEP HIBERNATEIDLE 
$hibernateValue = $hibernate| Select-String "Current AC Power Setting Index"
Write-Host "Sleep After:" $sleepValue.ToString().Split(":")[1]
Write-Host "Hibernate After:" $hibernateValue.ToString().Split(":")[1]


# A non working attempt by myself (tq84);


$plan = Get-cimInstance -Class win32_powerplan -Namespace root\cimv2\power  -Filter "isActive='true'" 
$regex = [regex]"{(.*?)}$"
$planGuid = $regex.Match($plan.instanceID.Tostring()).groups[1].value
# powercfg -query $planGuid


# $pwrSet = Get-cimInstance -Namespace root\cimv2\power  -Class win32_powerSetting  -filter "instanceId='Microsoft:PowerSetting\\{29f6c1db-86da-48c5-9fdb-f2b67b1f44da}'"

# $config = $pwrSet | where-object {$_.instanceId -eq 'Microsoft:PowerSetting\{29f6c1db-86da-48c5-9fdb-f2b67b1f44da}'}

$config = Get-cimInstance -Namespace root\cimv2\power  -Class win32_powerSetting  -filter "instanceId='Microsoft:PowerSetting\\{29f6c1db-86da-48c5-9fdb-f2b67b1f44da}'"



<#
Caption        :
Description    : Specify how long your computer is inactive before going to sleep.
ElementName    : Sleep after
InstanceID     : Microsoft:PowerSetting\{29f6c1db-86da-48c5-9fdb-f2b67b1f44da}
PSComputerName :
#>
