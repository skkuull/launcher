$cefVersion = 'cef_binary_91.1.21+g9dd45fe+chromium-91.0.4472.114_windows64'
#-------------------------------------------------

$cefPath = 'deps/cef'
Remove-Item -LiteralPath $cefPath -Force -Recurse -ErrorAction Ignore

Add-Type -AssemblyName System.Web
$cefURLVersion = [System.Web.HttpUtility]::UrlEncode($cefVersion) 

$source = "https://cef-builds.spotifycdn.com/$cefURLVersion.tar.bz2"
$destinationPart = 'cef.tar'
$destination = "$destinationPart.bz2"
Invoke-WebRequest -Uri $source -OutFile $destination

$sz = '"C:\Program Files\7-Zip\7z.exe"'

cmd /c "$sz x $destination -aoa"
cmd /c "$sz x $destinationPart -aoa"

Move-Item -Path $cefVersion -Destination $cefPath

Remove-Item $destinationPart
Remove-Item $destination