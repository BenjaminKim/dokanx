== This is a fork of dokan.

=== Build Setting
* Download and install the WDK 7.1.0 at here [http://www.microsoft.com/en-us/download/details.aspx?id=11800]
* You should set the Windows environment variables DOKANX_PATH and WIN7BASE before compile driver.
ex)
DOKANX_PATH=D:\dokanx
WIN7BASE=C:\WinDDK\7600.16385.1

== Improved
1. I have made a Visual Studio(2012) solution file. Now, you can compile filesystem driver easily.
2. dokan library dll was written by C. Now, you can use C++.
3. Original dokan provided mirrorfs to test the library. I will write virtual file system. It would be better than mirrorfs.
4. Using precompiled header for driver.
5. Usermode filesystem implementations can return therir result code as NTSTATUS(Not win32 anymore)
