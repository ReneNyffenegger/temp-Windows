#
#   Assign 'continue' to preference variable $verbosePreference
#   for verbose output.
#

set-strictMode -version 2

function upload-file-to-github-repo($localPath, $localFilename, $repoPath, $repoFilename, $repoName, $repoOwner, $message, $token) {
   #
   #      File content must be represented in base 64
   #

   $localFileAbsolutePath = resolve-path "$localPath/$localFilename"

   if (! (test-path $localFileAbsolutePath)) {
      "$localFileAbsolutePath does not exist"
       return
   }

   $base64 = [System.Convert]::ToBase64String([System.IO.File]::ReadAllBytes($localFileAbsolutePath))

   $url = "https://api.github.com/repos/$repoOwner/$repoName/contents/$repoPath/$repoFilename"

   try {

    #
    #     TODO: PowerShell 7 introduces option -skipHTTPErrorCheck
    #           This option should probably be used in order to have
    #           see what HTTP status code was returned.
    #

    # github API token is required for private repositories

      $response = invoke-restMethod                      `
         $url                                            `
        -headers @{Authorization = "bearer $token"}

   }
   catch [System.Net.WebException] {
     #
     #  I am too lazy to evaluate the details and just assume
     #  that this is a 404. So the resource needs to be created
     #
        write-verbose 'Resource probably does not exist. Trying to create it'

        $body = '{{"message": "{0}", "content": "{1}" }}' -f $message, $base64

     #
     #  TODO: of course, if the url $url does not exist on the webserver,
     #  the following invocation fails.
     #
        $response = invoke-webrequest $url                                               `
          -method          PUT                                                           `
          -contentType     application/json                                              `
          -headers       @{Authorization = "bearer $token"}                              `
          -body            $body

      #
      # expected: 201 Created
      #
        write-verbose "$($response.StatusCode) $($response.StatusDescription)"
        return
   }
   write-verbose "file seems to exit, updating it"
   $sha = $response.sha
   write-verbose "SHA = $sha"

   $body = '{{"message": "{0}", "content": "{1}", "sha": "{2}" }}' -f $message, $base64, $sha

   $response = invoke-webrequest $url                                               `
     -method          PUT                                                           `
     -contentType     application/json                                              `
     -headers       @{Authorization = "bearer $token"}                              `
     -body            $body
}
