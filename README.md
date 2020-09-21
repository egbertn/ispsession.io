#Introduction 
ISP Session enables classic asp (COM) websites, asp.net (aspx) and asp.net core websites to share the same session state, using a storage provider based on Redis.
This project just has become OPEN SOURCE, so it needs cleaning up at time of writing.

ISP Session started as a COM component in Visual Basic 6 providing as a classic asp page session alternative for the built-in memory based Microsoft IIS provided session state. This component used to be so stable that after 10 years customers came for an upgrade (because of Legacy reasons).

Originally being called ASP Session by Nierop Webconsultancy, there were quite some components being called 'ASP Session' so therefore, it was renamed to ISP Session. Also having in mind, that ISP Session supported multiple applications (using application key and session key).

ISP Session was rewritten using C++ and ATL using Visual C++ 6.0 and has been maintained during version upgrades of IIS and Windows up to Windows 2012.

Also, a compatible version for .NET was made, which allowed companies to share the same session using asp and aspx pages. However, this solution was not very scalable and not using the right approach.

In 2016 a .NET Framework 4.5 *module* was developed and later when ASP.NET Core 2.0 was released, the same was done for ASP.NET Core session state. However, session state is old-fashioned these days since that limits scalability, and has the known security implications. Still there are **valid reasons** to have a session state in some cases, such as when you have a 'signup' feature or anonymous experience needing some kind of state. 

When Windows 2019 was released, it was very clear that classic ASP is out of scope for new projects. ISP Session also badly needed a new market approach, being (free) open source and the services provided for it, invoiced.  

Apology: Since this project has a long history and some of the coding is using old coding styles, and lacking documentation, you'll see some funny things but I hope that contribution and co-operation to this project will improve a lot.

#Getting Started
Installation process

2.	Setting up Redis
2.	Using the binaries
3.	Compiling 

#Build and Test
At the time of writing no CI/CD has been implemented.
For nuget packaging this needs to be deployed automatically each time a PR has been executed.

#Contribute
You are welcome to contribute to this project. Things in mind to develop are e.g. 

1. encryption so keys and values are stored encrypted in Redis.
2. Support for other noSQL storages are welcome such as memcached.

