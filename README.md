# libcoreservices [![Build Status](https://travis-ci.org/PureDarwin/libcoreservices.svg?branch=master)](https://travis-ci.org/PureDarwin/libcoreservices)

This repository contains an implementation of PureDarwin's `libcoreservices` library, which contains code that locates system resources on disk, as well as creates application-specific temporary directories on request. This second function also requires the `dirhelper` daemon, which is also included.

This repository also contains a version of `libCrashReporterClient`, which is commonly used by PureDarwin components to supply an error message that will appear in crash logs (once crash reporting is implemented).

### Sources Used

```
- `NSSystemDirectories.c': Taken from Apple's Libc project (version
                           997.90.3 - released as part of OS X 10.9.5)
- `NSSystemDirectories.h': Taken from Apple's Libc project (version
                           997.90.3 - released as part of OS X 10.9.5)
- `sysdir.c':              Written by author
- `sysdir.h':              Copied from the macOS SDK in Xcode 9.2
                           (build 9C40b).
```
