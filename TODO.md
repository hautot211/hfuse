- Add documentation
- Implement
    - Failure when mounting non standard HFS filesystem
    - Virtual directory and virtual files for finder informations
- Rewrite
    - file handlers again (OO style, virtual classes)
- valgrind              OK (31/05/25)
- Unit tests ?
- Debug
    - 'find mnt -type f |xargs -n1 -d'\n' cat' killing program when runned in multithreaded mode
        - Might be caused by libhfs
        - Works well in single-threaded
    
- parse options

