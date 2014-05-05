# DokanX

## This is a fork of dokan 0.6.0
Because the original dokan isn't maintained anymore, I forked it.   
If you don't know about dokan, you should read this document. http://dokan-dev.net/en/docs/

## Build Setting
* Download and install the [WDK 7.1.0](http://www.microsoft.com/en-us/download/details.aspx?id=11800)
* You should set the Windows environment variables DOKANX_PATH and WIN7BASE before compile driver.

```bash
# Note. Do not enclose WIN7BASE environment value in double quotes. ddkbuild can't recognize it, I think.
DOKANX_PATH=D:\dokanx
WIN7BASE=C:\WinDDK\7600.16385.1
```

Even if you don't have Visual Studio, you can also build `dokanx` from WDK toolkit.  
WDK 8.x are not supported yet. But this doesn't mean dokan can't run on Windows 8 or later. 

## Improved
* Visual Studio 2013 solution file is provided with makefile and ddkbuild. Now you can compile filesystem driver very easily.
* All dokan library dll code has been recompiled in C++
* Using precompiled header for driver. It makes building driver much faster.
* Your usermode filesystem implementation should return their result code as NTSTATUS(Not the win32 status code).
This gives your application more control.
* Use better logger.
* Applied prefast analyzer(static analyze driver source code)
