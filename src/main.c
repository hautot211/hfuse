#include <stdio.h>
#include <stdlib.h>
#include <hfs.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>


int main() {
    hfsvol* volume = hfs_mount("/home/guillaume/Documents/fuse-test/samples/MacOS-9.0.4-fresh.rom", 0, HFS_MODE_RDONLY);

    hfsvolent* volume_entity = (hfsvolent*) malloc(sizeof(hfsvolent));
    hfs_vstat(volume, volume_entity);
    printf("%s\n", volume_entity->name);
    free(volume_entity);

    hfs_umountall();
}
