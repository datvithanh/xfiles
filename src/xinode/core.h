#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int inode_std_print(const char *fname)
{
  struct stat var;
  int ret=stat(fname,&var);

  if(ret<0)
    printf("some error happened");
  else
  {
    printf("Device id: \t%ld\n", var.st_dev);
    printf("Inode number: \t%ld\n", var.st_ino);
    printf("Mode: \t%ud\n", var.st_mode);
    printf("UID: \t%ud\n", var.st_uid);
    printf("GID: \t%ud\n", var.st_gid);
    printf("Size: \t%ld\n", var.st_size);
    printf("Number of Links: \t%ld\n", var.st_nlink);
    printf("Time of last access: \t%ld\n", var.st_atime);
    printf("Time of last data modification: \t%ld\n", var.st_mtime);
    printf("Time of last status change: \t%ld\n", var.st_ctime);
  }
}