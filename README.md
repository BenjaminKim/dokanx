# DokanX

## This is a fork of dokan 0.6.0
Because the original dokan isn't maintained anymore, I forked it.   
If you don't know about dokan, you should read this document. http://dokan-dev.net/en/docs/

## Build Setting
* Download and install the [WDK 7.1.0](http://www.microsoft.com/en-us/download/details.aspx?id=11800)
* You should set the Windows environment variables DOKANX_PATH and WIN7BASE before compile driver.

```
DOKANX_PATH=D:\dokanx
WIN7BASE=C:\WinDDK\7600.16385.1
```

WDK 8.0 are not supported yet. But this doesn't mean dokan can't run on Windows8. 

## Improved
* Visual Studio 2012 solution file is provided with makefile and ddkbuild. Now you can compile filesystem driver very easily.
* All dokan library dll code has been recompiled in C++
* Using precompiled header for driver.
* The Usermode filesystem implementations should return their result code as NTSTATUS(Not the win32 statua code anymore).
This gives your application more control.
