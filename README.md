# hfuse

A fuse based read-only filesystem for accessing *standard* HFS disk images, reading both data and resource forks.

## Features and limitations

- Can read resource forks (their is a virtual directory named '.rsrc' in all directory of the FS tree which contains resources of the files of the parent directory)
- Read-only
- Cannot read Finder informations for now


## Requirement

- cmake
- make
- libfuse3 (dev)
- [libhfs](https://github.com/hautot211/hfsutils)

## Installation

```bash
git clone https://github.com/hautot211/hfuse
cd hfuse
cmake .
make
sudo make install
```
