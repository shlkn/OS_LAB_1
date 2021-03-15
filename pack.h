#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

struct file_descriptor
{
	long int fl_sz; // file size 
	char fl_name[50]; //file name
	int is_reg; // 1 - regular file 0 - dir
	unsigned int id; // file id
	unsigned int pr_id; // parents id
};

void pack(char *file_names_arr[], int arg_cnt, int start_index); // manage to pack data in archive
void handle_file(int arch, int *file_cnt, char *path, char *file_name, int pr_id); // function that handle one regular file. makes descriptor and write it in archive
void handle_dir(int arch, int *file_cnt, char *path, char *dir_name, int pr_id); // recursive function that handle dir. makes descriptor and write it in archive
void copy_file(int arch, char *file_name); // copy file data in file in archive
void copy_data(int arch, char *path); // recursive function that handle dir. calls "copy_file" for files if it`s find it 