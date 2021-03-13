#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int file_cnt = 0; //count of struct

struct file_descriptor
{
	long int fl_sz; // file size 
	char fl_name[50]; //file name
	int is_reg; // 1 - regular file 0 - dir
	unsigned int id; // file id
	unsigned int pr_id; // parents id
};

void rec(int arch, char *dir_name, int pr_id)
{
	DIR *dp;
	struct dirent *cr_dir;
	struct file_descriptor dir_desc;
	struct stat fl_stat;
	dp = opendir(dir_name);
	if(dp == NULL)
		printf("Error can`t open dir\n");
	int file, cr_pr_id = pr_id;
	dir_desc.is_reg = 0; // type - dir
	dir_desc.fl_sz = 0; // writing size of dir - 0
	strcpy(dir_desc.fl_name, dir_name); // writing file name
	dir_desc.id = file_cnt; // writing file id
	dir_desc.pr_id = cr_pr_id; // writing parents id
	printf("         home dir     \n");
	printf("-----------------------------------------------------------\n");
	printf("file type - %d\n", dir_desc.is_reg);
	printf("file size - %ld\n", dir_desc.fl_sz);
	printf("file name - %s\n", dir_desc.fl_name);
	printf("file id - %d\n", dir_desc.id);
	printf("file parents id - %d\n", dir_desc.pr_id);
	printf("-----------------------------------------------------------\n");
	file_cnt++;
	while(1)
	{
		cr_dir = readdir(dp);
		if(cr_dir == NULL)
		{
			//break;
			printf("empty\n");
			return;
		}
		if(strcmp(cr_dir->d_name, ".") == 0 || strcmp(cr_dir->d_name, "..") == 0)
			continue; 
		//printf("%d file name %s\n", cr_dir->d_type, cr_dir->d_name);
		if(cr_dir->d_type == 8) // если просто файл
		{
			struct file_descriptor fl_desc;
			fl_desc.is_reg = 1; // type - regular file
			lstat(cr_dir->d_name, &fl_stat);
			fl_desc.fl_sz = fl_stat.st_size; // writing size of file
			strcpy(fl_desc.fl_name, cr_dir->d_name); // writing file name
			fl_desc.id = file_cnt; // writing file id
			fl_desc.pr_id = dir_desc.id; // writing parents id
			file_cnt++;
			printf("         file in dir     \n");
			printf("-----------------------------------------------------------\n");
			printf("file type - %d\n", fl_desc.is_reg);
			printf("file size - %ld\n", fl_desc.fl_sz);
			printf("file name - %s\n", fl_desc.fl_name);
			printf("file id - %d\n", fl_desc.id);
			printf("file parents id - %d\n", fl_desc.pr_id);
			printf("-----------------------------------------------------------\n");
		}
		else if(cr_dir->d_type == 4) // если директория
		{
			char buff[100];
			strcpy(buff, dir_name);
			//printf("dir - %s\n", cr_dir->d_name);
			strcat(buff, "/");
			strcat(buff, cr_dir->d_name);
			printf("%s\n", buff);
			rec(arch, buff, dir_desc.id);
		}
	}
}

int main(int argc, char* argv[])
{
	int arch;
	if((arch = open("my_arch", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		printf("Error, can`t create archive file \n");
	write(arch, &file_cnt, sizeof(int));
	rec(arch, argv[1], file_cnt);
}