#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "record.h"


int main(int argc, char** argv)
{
    int fd1[2], i, l = 2, j,z=0, status, d, error = 0, size_of_list = 0;
    char buffer1[24], buffer2[24];
    info_pointer list_child = NULL, list_parent = NULL;
    pid_t pid;
    char buf[20];
    char temp[10];
    
    int numOfSMs = atoi(argv[2]);

    for(i = 0; i < numOfSMs; i++)
    {
        sprintf(buf, "fifo%s%d", argv[3],i); 
        if (mkfifo(buf, 0666) < 0)
            perror("mkfifo");
        if (pipe(fd1) == -1) perror ( " pipe call ");
        switch(pid = fork())
        {
            case 0:
                create_file(argv[3], numOfSMs, i, temp, 's', &error);
                if (error == 1) printf("error creating new file\n");

                size_of_list = fifo_read(&list_child, buffer1, buffer2, buf, &error);

                if( size_of_list != 0)
                {
                    sort_list(&list_child, &error);
                    if (error == 1) printf("error at sorting list\n"); 
                    
                    child_write_to_parent(&list_child, fd1[0], fd1[1], buffer1, buffer2, &error);
                    if(error == 1) printf("splitter-error child write to parent\n");
                }    

                if (i == (numOfSMs - 1 )) j = remove(argv[3]);
            //  if (j == 0 ) printf("file removed %s\n", argv[3]);
            //  else printf("file not removed\n");
                
                
                execlp("./sorters", "sorters", argv[1], argv[2], temp, buf, NULL);
                perror("exec failure");
                 
                exit(1);   
            case -1:
            // take care of possible fork failure 
                perror("fork failure");
                break;
            default:
                size_of_list = parent_read_from_child(&list_parent, fd1[0], fd1[1], buffer1, buffer2, &error);
                if(error == 1) printf("splitter-error parent read from child\n");
 
                fifo_write(&list_parent, argv[4], &error);
                if(error == 1) printf("splitter-error fifo write\n");
                
                while ((pid = wait(&status)) > 0);                 
        }    
    }
    
    
    return (EXIT_SUCCESS);
}

