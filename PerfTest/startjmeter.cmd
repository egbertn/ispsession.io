@del /S /Q .\results\*.* 
@rmdir /Q /S .\results
del results.txt
C:\Develop\apache-jmeter-5.2.1\bin\jmeter.bat -n -t starttest.jmx -l results.txt -e -o .\results 
