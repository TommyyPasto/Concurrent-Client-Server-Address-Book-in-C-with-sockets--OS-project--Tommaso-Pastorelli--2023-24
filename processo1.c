#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{

    FILE * file;
    int fsize, offset;
    char buf[50];

    // Open the file that is passed as a command line argument to the program in Read and Write Mode
    if ((file = fopen("filetest.txt", "r+")) == -1) // if Open is not successful exit from the program else proceed
    {
        perror("can't open file");
        exit(1);
    }
    
    fseek(file, 0L, SEEK_END);
    fsize = ftell(file); 
    printf("size: %d", fsize);

    /* Utype whence Ustart len 1pid */
    struct flock * fl = malloc(sizeof(struct flock));
    fl->l_type = F_RDLCK;
    fl->l_whence = SEEK_SET;
    fl->l_start = 0;
    fl->l_len = fsize;
    fl->l_pid = getpid();

    int fd = fileno(file);

    printf("File is Not Locked by any Process\n");
    printf("Press Enter to Lock the File\n");

    getchar(); // gets a key hit from the user, it can be any key including Enter Key

    if (fcntl(fd, F_SETLKW, fl) == -1) // If not able to lock the file exit, else proceed
    {
        perror("can't set Exclusive Lock");
        exit(1);
    }
    else if (fl->l_type != F_UNLCK) // if the file is locked print the process id which is holding the locking
    {
        printf("File has been Exclusively Locked by process: %d\n", fl->l_pid);
    }
    else
    {
        printf("File is not Locked\n");
    }
    printf("Press ENTER to Release lock:\n");
    getchar();
    fl->l_type = F_UNLCK; // Replace the F_WRLCK to F_UNLCK to unlock the file
    printf("File has been Unlocked\n");

    
}
