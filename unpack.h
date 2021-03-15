#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

struct unpack_file_descriptor
{
	long int fl_sz; // file size 
	char fl_name[50]; //file name
	int is_reg; // 1 - regular file 0 - dir
	unsigned int id; // file id
	unsigned int pr_id; // parents id
};

void unpack(char *file_name); // manage to unpack data in archive.
void unpack_dir(struct unpack_file_descriptor *fl_desc); // makes dir
void unpack_file(int arch, struct unpack_file_descriptor *fl_desc); // makes file and fills it


