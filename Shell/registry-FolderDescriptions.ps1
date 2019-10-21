echo "digraph SHELL_NAMESPACE {" > shell_namespace.dot

foreach ($subkey in (get-childItem hklm:\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FolderDescriptions) ) {
# echo $subkey.getValue('name')
  $clsid    = $subkey.psChildName.toUpper()
  $name     = $subkey.getValue('name')
  $nodeName = 'clsid_' + $clsid.replace('-','').replace('{', '').replace('}','')
  echo "$nodeName [ label = <table border= `"0`"><tr><td>$name</td></tr> </table> ] " >> shell_namespace.dot
}
