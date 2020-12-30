#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

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
    printf("UID: \t%u\n", var.st_uid);
    printf("GID: \t%u\n", var.st_gid);
    //printf("Mode: \t%ud\n", var.st_mode);
    
    printf("\n");
    
    printf("Size: \t%ld bytes\n", var.st_size);
    printf("Number of Links: \t%ld\n", var.st_nlink);
    printf("Time of last access: \t%s", asctime(gmtime(&var.st_atime)));
    printf("Time of last data modification: \t%s", asctime(gmtime(&var.st_mtime)));
    printf("Time of last status change: \t%s", asctime(gmtime(&var.st_ctime)));
    
    printf("\n");
    
    printf("File Type: ");
    if (S_ISDIR(var.st_mode))
    	printf("Directory");
    else if (S_ISCHR(var.st_mode))
    	printf("Character Device");
    else if (S_ISBLK(var.st_mode))
    	printf("Block Device");
    else if (S_ISREG(var.st_mode))
    	printf("Regular File");
    else
    	printf("<unknown>");
    printf("\nFile Permissions: \t");
    printf( (S_ISDIR(var.st_mode)) ? "d" : "-");
    printf( (var.st_mode & S_IRUSR) ? "r" : "-");
    printf( (var.st_mode & S_IWUSR) ? "w" : "-");
    if ( var.st_mode & S_ISUID )
    	printf( (var.st_mode & S_IXUSR) ? "s" : "S");
    else
    	printf( (var.st_mode & S_IXUSR) ? "x" : "-");
    printf( (var.st_mode & S_IRGRP) ? "r" : "-");
    printf( (var.st_mode & S_IWGRP) ? "w" : "-");
    if ( var.st_mode & S_ISGID )
    	printf( (var.st_mode & S_IXGRP) ? "s" : "S");
    else
    	printf( (var.st_mode & S_IXGRP) ? "x" : "-");
    printf( (var.st_mode & S_IROTH) ? "r" : "-");
    printf( (var.st_mode & S_IWOTH) ? "w" : "-");
    printf( (var.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    
  }
}
