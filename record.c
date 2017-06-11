#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "record.h"


typedef struct type_node{
    char name[24];
    int sum_of_votes;
    type_pointer next;
}type_node;

typedef struct info_node{
    int size;  
    type_pointer start;    
}info_node;


void create_file(char *fp, int numOfSMs, int n, char *temp,char c, int *error)
{
    int fd, i;
    record rec;
    long lSize;
    int numOfrecords;
    int number_of_lines;
    FILE *temp_file, *fpb;
    
    fpb = fopen (fp,"rb");
    if (fpb == NULL) {
        printf("Cannot open binary file\n");
        *error =  1;
    }
    
    // check number of records
    fseek (fpb , 0 , SEEK_END);
    lSize = ftell (fpb);
    rewind (fpb);
    numOfrecords = (int) lSize/sizeof(rec);

    
    number_of_lines = numOfrecords / numOfSMs;
    
    sprintf(temp, "%c%d%s", c, n, fp);
    if(n != 0)                          //Find the lines we need
    {
        n = n * number_of_lines;
        if((n * 2) >= numOfrecords) number_of_lines = numOfrecords;
        else number_of_lines = n * 2;
    }
    else    n = 0; 

    temp_file = fopen( temp , "w");
    
    for (i = 0; i < numOfrecords ; i++) {
        fread(&rec, sizeof(rec), 1, fpb);
        if (i >= n && i < number_of_lines) fwrite(&rec, 1, sizeof(rec), temp_file);              
      //  printf("%s %hd %c\n", rec.name, rec.electionCenter, rec.valid);             
    }   
    fclose(temp_file);
    fclose(fpb);    
    return ;  
}

info_pointer create_list()              //Create simple list
{
    info_pointer temp = malloc(sizeof(info_node));
    if ( temp == NULL )  return;   
    temp->size = 0;
    temp->start = NULL;       
    return temp;
}


int list_entry(info_pointer *  const list,record rec, int *error)
{
    int flag = 0;
    type_pointer temp, pointer;
    *error = 0;
    pointer = (*list)->start;
    temp = malloc(sizeof(type_node));
    if ( temp == NULL)    
    {   *error = 1;
        return;
    }
    if (rec.valid != '0')  //Vote is not valid
        strcpy(temp->name, rec.name);
    if ((*list)->start == NULL)                    //First entry
    {
        temp->next = (*list)->start;
        (*list)->start = temp;  
        temp->sum_of_votes = 1;
    }
    else{
        while(pointer->next != NULL)             //While you aren't at the end of the list
        {
            if (strcmp(temp->name,pointer->name) == 0)   //If candidate exists
            {
                pointer->sum_of_votes ++;                       //increase the votes he got
                flag = 1;
                free(temp);
                break;
            }
            else
                pointer = pointer->next;
        }
        if (flag == 0)
        {
            if (strcmp(temp->name,pointer->name) == 0)           //candidate at the end of the list
            {
                pointer->sum_of_votes ++;
                free(temp);
            }      
            else                         //candidate doesn't exist so enter him at the end of the list
            {
                pointer->next = temp;
                temp->sum_of_votes = 1;     //First vote
                temp->next = NULL;
            }
        }
    }
   return (*list)->size ++;    
}


void LIST_diadromi(const info_pointer  linfo, int * const error)
{     
      char * str;
	type_pointer temp;
	temp = (linfo)->start;
	*error = 0;
	
	if(linfo->start == NULL)
	{	*error=1;
		return;
	}
	
	while(temp!=NULL)
	{   
            sprintf(str, "%d", temp->sum_of_votes);
            printf("candidate %s votes %d\n",temp->name, temp->sum_of_votes);
            temp = temp->next;
	}
        return;
}


void sort_list(const info_pointer  *list, int * const error)
{
    type_pointer temp, pointer;
    *error = 0;
    if((*list)->start == NULL)
    {	
        *error = 1;
	return;
    }
    temp = (*list)->start;
    while(temp->next != NULL)       //Sort given list comparing names
    {
        if(strcmp(temp->name,temp->next->name) > 0) 
        {
            strcpy(pointer->name,temp->name);
            pointer->sum_of_votes = temp->sum_of_votes;
            strcpy(temp->name, temp->next->name);
            temp->sum_of_votes = temp->next->sum_of_votes;
            strcpy(temp->next->name, pointer->name);
            temp->next->sum_of_votes = pointer->sum_of_votes;
        }
        temp = temp->next;
    }
    return;
}


void set_value(const info_pointer  *list, char str1[], char str2[], int *error)
{
    type_pointer temp,pointer;
    *error = 0;
    if ((*list)->start == NULL) *error = 1;
    temp = (*list)->start;
    strcpy(str1,temp->name);
    sprintf(str2, "%d", temp->sum_of_votes);
    pointer = temp;
    temp = temp->next;
    free(pointer);
    (*list)->size --;
    (*list)->start = temp;
    return;
}

void LIST_destruct(info_pointer * linfo)
{
    type_pointer todel,todel2;
    todel = (*linfo)->start;
    while(todel != NULL)
    {   todel2 = todel;
	todel = (todel)->next;
	free(todel2);
    }
    (*linfo)->start = NULL;
    free(*linfo);
    (*linfo)=NULL;
}

int list_entry_parent(info_pointer *  const list,char str1[], char str2[], int *error)
{
    int flag = 0;                       
    type_pointer temp, pointer; 
    *error = 0;
    pointer = (*list)->start;
    temp = malloc(sizeof(type_node));
    if ( temp == NULL)    
    {   *error = 1;
        return;
    }
    strcpy(temp->name, str1);
    temp->sum_of_votes = atoi(str2);
    //printf("%s %d\n",temp->name, temp->sum_of_votes);
    if ((*list)->start == NULL)                    //First entry
    {
        temp->next = (*list)->start;
        (*list)->start = temp;  
    }
    else{
        while(pointer->next != NULL)             //While you aren't at the end of the list
        {
            if (strcmp(temp->name,pointer->name) == 0)   //If candidate exists
            {
                pointer->sum_of_votes += temp->sum_of_votes;       //increase the votes he got
                flag = 1;
                free(temp);
                break;
            }
            else
                pointer = pointer->next;
        }
        if (flag == 0)
        {
            if (strcmp(temp->name,pointer->name) == 0)           //candidate at the end of the list
            {
                pointer->sum_of_votes += temp->sum_of_votes;
                free(temp);
            }      
            else                        //candidate doesn't exist so enter him at the end of the list
            {
                pointer->next = temp;
                temp->next = NULL;
            }
        }
    }   
   return (*list)->size ++;
}


void child_write_to_parent(info_pointer *list,int fd1, int fd2,char str1[], char str2[],  int *error)
{
    int i;
    *error = 0;
    close(fd1);                         //Child process closes input side of the pipe 
    dup(fd2);                           //Connect pipe to stdout
    
    for (i = 0; i < (*list)->size ; i++) 
    {
        set_value(list,str1,str2,error);  //Get the candidate and hiw votes from the list
        if(*error == 1) return;
        write(fd2, str1, (strlen(str1)+1));  //Send "string" through the output side of pipe 
        write(fd2, str2, (strlen(str2)+1));
    }
    close(fd2) ;                    //Child process closes output side of the pipe 
    return;
}


int parent_read_from_child(info_pointer *list,int fd1, int fd2,char str1[], char str2[],  int *error)
{
    int size = 0;
    char buffer1[24], buffer2[24];
    *error = 0;
    close(fd2);                     //Parent process closes output side of the pipe 
    
    while(read(fd1, buffer1, (strlen(str1)+1)) > 0 )      //Get "string" from the intput side of pipe
    {
        read(fd1, buffer2, (strlen(str2)+1));
        size = list_entry_parent(list,buffer1, buffer2, error); //Enter candidate-votes in new parent list                  
    }
    close(fd1);             //Parent process closes input side of the pipe 
    sort_list(list, error); //Sort/merge list after entering all data from children
    return size;
}


int fifo_read(info_pointer *list,char str1[], char str2[],char buf[], int *error)
{
    int fd, size = 0;
    fd = open(buf, O_RDONLY | O_NONBLOCK  );    //Open fifo for reading if it is already opened for writing
    while(read(fd, str1, (strlen(str1)+1)) > 0)  //Read message from fifo
    {
        read(fd, str2, (strlen(str2)+1));
        size = list_entry_parent(list,str1, str2, error); 
    }
    close(fd);          //Parent process closes input side of the pipe 
    return size;
}

void fifo_write(info_pointer *list, char buf[], int *error)
{
    char str1[24],str2[24];
    int i, fd;
    fd = open(buf, O_WRONLY);   //Open fifo for writing
    
    for (i = 0; i < (*list)->size ; i++) //For every candidate-votes in the list
    {
        set_value(list,str1,str2,error);
        if(*error == 1) printf("error at set value-free node\n");
        write(fd, str1, (strlen(str1)+1));      //Write candidate to fifo
        write(fd, str2, (strlen(str2)+1));      //Write his votes to fifo
    }    
    close(fd);              //Child process closes output side of the pipe 
    unlink(buf);            //Remove fifo
    return;
}