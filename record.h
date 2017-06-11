#ifndef RECORD_H
#define	RECORD_H

typedef struct {
   char name[24];
   short electionCenter;
   char valid;
}record;

typedef struct info_node  * info_pointer;

typedef struct type_node *type_pointer;

void create_file(char *fp, int numOfSMs, int n, char *temp, char c,int *error);
info_pointer create_list();
int list_entry(info_pointer *  const list,record rec, int *error);
void LIST_diadromi(const info_pointer  linfo, int * const error);
void sort_list(const info_pointer  *list, int * const error);
void set_value(const info_pointer  *list, char str1[], char str2[], int *error);
void LIST_destruct(info_pointer * linfo);
int list_entry_parent(info_pointer *  const list,char str1[], char str2[], int *error);
void child_write_to_parent(info_pointer *list,int fd1, int fd2,char str1[], char str2[], int *error);
int parent_read_from_child(info_pointer *list,int fd1, int fd2,char str1[], char str2[],  int *error);
int fifo_read(info_pointer *list,char str1[], char str2[],char buf[],  int *error);
void fifo_write(info_pointer *list, char buf[], int *error);

#endif	/* RECORD_H */

