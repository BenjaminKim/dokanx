<html>
This is a fork of dokan.
<body>
<h3>Build Setting</h3>
You should set the Windows environment variables DOKANX_TRUNK and WIN7BASE before compile driver.<br />
ex)<br />
DOKANX_TRUNK=D:\dokanx<br />
WIN7BASE=C:\WinDDK\7600.16385.1<p>
<h3>Improved</h3>
1. I have made a Visual Studio(2010) solution file. Now, you can compile filesystem driver easily.<br />
2. dokan library dll was written by C. Now, you can use C++.<br />
3. Original dokan provided mirrorfs to test the library. I will write virtual file system. It would be better than mirrorfs.<br />
4. Using precompiled header for driver.<br />
5. Usermode filesystem implementations can return therir result code as NTSTATUS(Not win32 anymore)
</body>
</html>
