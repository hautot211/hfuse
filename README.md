# hfuse

A fuse based read-only filesystem for accessing *standard* HFS disk images, reading both data and resource forks.

## Features and limitations

- Can read resource forks (their is a virtual directory named '.rsrc' in all directory of the FS tree which contains resources of the files of the parent directory)
- Read-only
- Cannot read Finder informations for now


## Requirement

- gcc (C11 or later)
- cmake
- make
- libfuse3 (dev)
- [My forked libhfs](https://github.com/hautot211/hfsutils) (fixing some include files)

## Installation

If you are on Debian or ubuntu, run the install.sh file (requires libhfs to be installed).


## Some informations

- HFS authorizes filenames containing '/', but Linux don't. In that case the program will transform '/' into '%2F'
- Nothing else is converted, CR is not translated to LF.
- Symlinks are distinguished from normal files but are not linked to any HFS entity for now.
- This program runs in single-thread by default because of issues when using libhfs in multithreaded mode.


