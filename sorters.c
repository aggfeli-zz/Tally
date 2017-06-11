#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "record.h"

void handler();

int main(int argc, char** argv) 
{    
    int fd1[2], i, j, status, error = 0, size_of_list = 0, sum = 0;
    char temp[10], str1[24],str2[24];
    long lSize;   
    info_pointer list_child = NULL, list_parent = NULL;
    record rec;
   // struct tms tb1, tb2;
    double t1, t2, cpu_time, ticspersec;

    FILE *fpb;
    int numOfSMs = atoi(argv[2]);
    pid_t pid;
    
    list_parent = create_list();
    
    for(i = 0; i < numOfSMs; i++)
    { 
        if (pipe(fd1) == -1) perror ( " pipe call ");
        switch(pid = fork())
        {
            case 0:
                //child process
                ticspersec = (double) sysconf(_SC_CLK_TCK);
            //    t1 = (double) times(&tb1);


                create_file(argv[3], numOfSMs, i, temp, 'w', &error);
                if (error == 1) printf("error creating new file\n");

                list_child = create_list();

                fpb = fopen (temp,"rb");
                if (fpb == NULL) {
                    printf("Cannot open binary file\n");
                    error =  1;
                }

                fseek (fpb , 0 , SEEK_END);
                lSize = ftell (fpb);
                rewind (fpb);
                int numOfrecords = (int) lSize/sizeof(rec);
                
                for (j = 0; j < numOfrecords ; j++) 
                {
                    fread(&rec, sizeof(rec), 1, fpb);     
                    size_of_list = list_entry(&list_child,rec, &error);
                    if (error == 1) printf("error while entering candidate at the list\n");             
                }   
                sort_list(&list_child, &error);
                if (error == 1) printf("error at sorting list\n");               

                fclose (fpb);
                j = remove(temp);
                //if (j == 0 ) printf("file removed %s\n", temp);
               // else printf("file not removed\n");
                if (i == (numOfSMs - 1)) j = remove(argv[3]);
                //if (j == 0 ) printf("file removed %s\n", argv[3]);
                //else printf("file not removed\n");
                
                child_write_to_parent(&list_child, fd1[0], fd1[1],str1, str2,  &error);
                if(error == 1) printf("sorter-error child write to parent\n");
            //    t2 = (double) times(&tb2);
            //   cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) -(tb1.tms_utime + tb1.tms_stime));
                
                signal(getppid(), handler);
                exit(0);
                    
            case -1:
            // take care of possible fork failure 
                perror("fork failure");
                break;
            default:
            // wait for child/parent to synchronize 
                size_of_list = parent_read_from_child(&list_parent, fd1[0], fd1[1], str1, str2, &error);
                if(error == 1) printf("sorter-error parent read from child\n");

                fifo_write(&list_parent, argv[4], &error);
                if(error == 1) printf("sorter-error fifo write\n");
                
                kill(getpid(), SIGUSR1);
                while ((pid = wait(&status)) > 0);
        }
    }
    
    return (EXIT_SUCCESS);
}

void handler()
{ 
    signal(SIGUSR1,handler);
}

