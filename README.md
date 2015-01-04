# DokanX

## Dokanx is a Fork of Dokan 0.6.0
Because the original Dokan is no longer maintained, I have decided to fork it.   
You can read about the original Dokan at [dokan-dev.net/en/docs/](http://dokan-dev.net/en/docs/) . 

## What is Improved
* A Visual Studio 2013 solution file is provided along with makefile and ddkbuild, so you can build more easily.
* All Dokan library dll code has been recompiled in C++
* Uses a precompiled header for the driver, making building much faster.
* Usermode filesystem implementations return their result code as an NTSTATUS (not a win32 status), giving applications more control.
* Uses a better logger.
* The PREfast analyzer (static analyzing driver source code) has been applied.

## To Build
* Download and install the [WDK 7.1.0](http://www.microsoft.com/en-us/download/details.aspx?id=11800)
* Set the Windows environment variables DOKANX_PATH and WIN7BASE before compiling the driver.

```bash
# Note. Do not enclose the WIN7BASE environment value in double quotes.  
# It seems ddkbuild can't handle this.
DOKANX_PATH=YOURWORKSPACE\dokanx
WIN7BASE=C:\WinDDK\7600.16385.1
```
* Choose build configuration `debug_win7` or `release_win7` regardless your actual target. The driver binary(.sys) will work on any target. 

### When your Build Succeeds, you will have...
* dokanx.dll
* dokanx.lib
* mirrorfs.exe
* dokanx_win7.sys
* dokanx_mount.exe
* dokanx_control.exe

WDK 8.x is not supported yet, but this doesn't mean Dokanx can't run on Windows 8 or later. 

## How to Mount Mirrorfs
### Registering and Starting the DokanX Filesystem Driver
Before mounting a volume with the filesystem, you need to *register* and *start* the filesystem driver.  
This can be done with [CreateService](http://msdn.microsoft.com/en-us/library/windows/desktop/ms682450(v=vs.85).aspx) and [StartService](http://msdn.microsoft.com/en-us/library/windows/desktop/ms686321(v=vs.85).aspx) functions. You could write your own code for registering/starting the driver if you want, maybe when your product is ready to deploy for end-users. At that time, [this simple wrapper](https://github.com/BenjaminKim/dokanx/blob/master/Common/WinNT/NtServiceCtrl.cpp) could be helpful to you.

But you don't need to deal with that just for testing; there is a simple tool for you.
You can easily *register* and *start* the Dokanx driver with the [osrloader](http://www.osronline.com/article.cfm?article=157) app.
* Move `dokanx_win7.sys` to `%SystemRoot%\System32\drivers\dokanx.sys`.
* Set this path in osrloader's `Driver Path` and write `dokanx` to `Display Name`
* Click `Register Service` and `Start Service`.

If there was no problem, you are ready to mount a volume.  
Note that the `Stop Service` function doesn't work. This may be `dokanx.sys`'s problem. (It is very hard to write device driver stopping code in Windows.) You need to reboot to stop the driver.

### Mounting a Volume
```
> dokanx_control.exe /i s
> mirrorfs.exe /l m /r c:\your_actual_folder
```

You should see that the M:\ drive has been mounted in Windows Explorer.

## How to Test Your Own Filesystem
1. Use [FileTest](https://github.com/ladislav-zezula/FileTest). This is the best tool to check a single filesystem operation.
2. Use [FileSystemTest](https://github.com/BenjaminKim/FileSystemTest). I made hundreds of test cases for filesystem operations. If you run this program on *your file system volume* **and** an *NTFS volume*, you can get input-output results for both file systems and you can find what is wrong with your file system by diffing these files. 
3. Use [ProcessMonitor](http://technet.microsoft.com/en-us/sysinternals/bb896645.aspx). If you are facing a new problem, ProcessMonitor is a great first tool to use. 
