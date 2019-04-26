# create the FtpWebRequest and configure it
#If ($args.Length == 0)
# {
# Write-Host "Invalid arguments"
# }
$ftp = [System.Net.WebRequest]::Create("ftp://ftp.adccure.nl/ispsession.io/wwwroot/Downloads/" + $args[0])
$ftp = [System.Net.FtpWebRequest]$ftp #cast
$ftp.Method = [System.Net.WebRequestMethods+Ftp]::UploadFile #"STOR"
$ftp.Credentials = new-object System.Net.NetworkCredential("en_1001","46Bd5f.=fdsd230")
$ftp.UseBinary = $true
$ftp.UsePassive = $true
$ftp.EnableSsl = $true
$fullPathIncFileName = $MyInvocation.MyCommand.Definition

$currentExecutingPath = [System.IO.Path]::GetDirectoryName($fullPathIncFileName)
$file = [System.IO.Path]::Combine($currentExecutingPath, $args[0])
Write-Host "Going To Copy " + $file
# read in the file as a stream. 
$content = [System.IO.File]::OpenRead($file)

# get the request stream, and copy the bytes into it
$rs = $ftp.GetRequestStream()
$content.CopyTo($rs)
$rs.Close()
$content.Close()
