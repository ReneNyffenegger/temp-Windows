$PSDefaultParameterValues = @{ '*:Encoding' = 'utf8' }

"digraph SHELL_NAMESPACE {" | out-file -encoding ASCII shell_namespace.dot
"  node [ shape=box fontname=Helvetica ]" | out-file -append -encoding ASCII shell_namespace.dot
"  rankdir=LR" | out-file -append -encoding ASCII shell_namespace.dot

foreach ($subkey in (get-childItem hklm:\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FolderDescriptions) ) {
# echo $subkey.getValue('name')
  $name        = $subkey.getValue('name')

  $clsid       = $subkey.psChildName.toUpper()
  $parentClsid = $subkey.getValue('parentFolder')
  $category    = $subkey.getValue('category')

  echo "category: $category"


  $node        = 'clsid_' +       $clsid.replace('-','').replace('{', '').replace('}','')
 "$node [ label = < <table border= `"0`"><tr><td>$name</td></tr><tr><td>$clsid</td></tr></table> > ] " | out-file -append shell_namespace.dot 

  if ($parentClsid) {
     $parentNode  = 'clsid_' + $parentClsid.replace('-','').replace('{', '').replace('}','')
    "$parentNode -> $node" | out-file -append shell_namespace.dot
  }
}

'}' | out-file -append -encoding ASCII shell_namespace.dot

$withBom = get-content shell_namespace.dot -raw
$withoutBom = new-object System.Text.UTF8Encoding $false
[System.IO.File]::WriteAllLines('shell_namespace.dot', $withBom, $withoutBom)
