#include <stdio.h>
#include <hfs.h>
#include <fuse.h>

int main() {
    hfsvol* volume = hfs_mount("/home/guillaume/Documents/fuse-test/samples/MacOS-9.0.4-fresh.rom", 0, HFS_MODE_RDONLY);
    printf("hfsvol ref : %lld\n", volume);
    printf("hello\n");
}
