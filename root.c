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
    int fd[2], fd1[2], i, j, status, error = 0, size_of_list = 0;    
    char temp[10], buffer1[24], buffer2[24],buf[20];
    info_pointer list_child = NULL, list_parent = NULL;
    FILE *f;
   
    //int depth = atoi(argv[6]);
    sprintf(argv[6], "%d", 3);              //depth = 3
    int numOfSMs = atoi(argv[4]);   
    pid_t pid;

    for(i = 0; i < numOfSMs; i++)
    {
        sprintf(buf, "fifo%s%d", argv[2],i); 
        if (mkfifo(buf, 0666) < 0)
            perror("mkfifo");
        if (pipe(fd1) == -1) perror ( "pipe call");
        switch(pid = fork())
        {
            case 0:
                create_file(argv[2], numOfSMs, i, temp, 'r', &error);
                if (error == 1) printf("error creating new file\n");

                size_of_list = fifo_read(&list_child, buffer1, buffer2, buf, &error); //Read from fifo

                if( size_of_list != 0) //If you have read from fifo and list is not empty 
                {
                    sort_list(&list_child, &error);     //Sort list 
                    if (error == 1) printf("error at sorting list\n"); 
                    //Child process pass list to parent with pipes
                    child_write_to_parent(&list_child, fd1[0], fd1[1], buffer1, buffer2, &error); 
                    if(error == 1) printf("root-error child write to parent\n");
                }
                
                execlp("./splitters_mergers", "splitters_mergers", argv[6], argv[4], temp, NULL);
                perror("exec failure");                                                                               
                exit(1);
            case -1:
            // take care of possible fork failure 
                perror("fork failure");
                break;
            default:
                
            //Get data from child process with pipes                 
            //size_of_list = parent_read_from_child(&list_parent, fd1[0], fd1[1], buffer1, buffer2, &error);
            //if(error == 1) printf("root-error parent read from child\n");
                if (size_of_list != 0)
                {
                    if (argv[10] != NULL) f = fopen(argv[10], "w");
                    for(j = 0; j < size_of_list; j++)
                    {                      
                        set_value(&list_parent,buffer1,buffer2,&error);
                        sprintf(buffer1, "%s %s", buffer1, buffer2);
                        printf("%s\n",buffer1);
                        if (argv[10] != NULL) fwrite(buffer1, 1, sizeof(buffer1), f);
                    }   
                    if (argv[10] != NULL) fclose(f);
                }
                while ((pid = wait(&status)) > 0);
        } 
    }

    return (EXIT_SUCCESS);
}

