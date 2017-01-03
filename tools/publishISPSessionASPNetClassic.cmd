"c:\program files (x86)\MSBuild\14.0\bin\MSBuild.exe" ..\src\CSessionManaged\ISPSessionClassicAsp.csproj /p:Configuration=Production;Platform=AnyCPU

rem ..\nuget sources update -Name "ADC Cure" -source https://nuget.adccure.nl/ -User admin -pass blaaaaaaaaaat

..\src\nuget.exe push ..\src\CSessionManaged\bin\Production\*.nupkg -Source https://nuget.adccure.nl/ -ApiKey 82C7D012-F532-4B00-B229-22B1168B86EE