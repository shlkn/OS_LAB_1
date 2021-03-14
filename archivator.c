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

void handle_file(int arch, int *file_cnt, char *path, char *file_name, int pr_id)
{
	struct stat fl_stat;
	struct file_descriptor fl_desc;
	fl_desc.is_reg = 1; // type - regular file
	lstat(path, &fl_stat);
	fl_desc.fl_sz = fl_stat.st_size; // writing size of file
	strcpy(fl_desc.fl_name, file_name); // writing file name
	fl_desc.id = *file_cnt; // writing file id
	fl_desc.pr_id = pr_id; // writing parents id
	(*file_cnt)++;
	printf("         file %s     \n", fl_desc.fl_name);
	printf("-----------------------------------------------------------\n");
	printf("file type - %d\n", fl_desc.is_reg);
	printf("file size - %ld\n", fl_desc.fl_sz);
	printf("file name - %s\n", fl_desc.fl_name);
	printf("file id - %d\n", fl_desc.id);
	printf("file parents id - %d\n", fl_desc.pr_id);
	printf("-----------------------------------------------------------\n");
	write(arch, &fl_desc, sizeof(struct file_descriptor));
	return;
}

void handle_dir(int arch, int *file_cnt, char *path, char *dir_name, int pr_id)
{
	DIR *dp;
	struct dirent *cr_dir, *cr_file;
	struct file_descriptor dir_desc;
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open dir\n");
		return;
	}
	int file, cr_pr_id = pr_id;
	dir_desc.is_reg = 0; // type - dir
	dir_desc.fl_sz = 0; // writing size of dir - 0
	strcpy(dir_desc.fl_name, dir_name); // writing file name
	printf("dir id - %d\n", *file_cnt);
	dir_desc.id = *file_cnt; // writing file id
	dir_desc.pr_id = cr_pr_id; // writing parents id
	printf("         file %s     \n", dir_desc.fl_name);
	printf("-----------------------------------------------------------\n");
	printf("file type - %d\n", dir_desc.is_reg);
	printf("file size - %ld\n", dir_desc.fl_sz);
	printf("file name - %s\n", dir_desc.fl_name);
	printf("file id - %d\n", dir_desc.id);
	printf("file parents id - %d\n", dir_desc.pr_id);
	printf("-----------------------------------------------------------\n");
	write(arch, &dir_desc, sizeof(struct file_descriptor));
	(*file_cnt)++;

	while(1)
	{
		cr_file = readdir(dp);
		if(cr_file == NULL)
			break;
		if(cr_file->d_type == 8) // если просто файл
		{
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_file->d_name);
			//printf("file - %s\n", full_path);
			handle_file(arch, file_cnt, full_path, cr_file->d_name, dir_desc.id);
		}
	}
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open dir\n");
		return;
	}
	while(1)
	{
		cr_dir = readdir(dp);
		if(cr_dir == NULL)
		{
			printf("empty\n");
			return;
		}
		if(strcmp(cr_dir->d_name, ".") == 0 || strcmp(cr_dir->d_name, "..") == 0)
			continue; 
		if(cr_dir->d_type == 4) // если директория
		{	
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_dir->d_name);
			//printf("dir - %s\n", full_path);
			handle_dir(arch, file_cnt, full_path, cr_dir->d_name, dir_desc.id);
		}
	}
}
void copy_file(int arch, char *file_name)
{
	int nread, file;
	char buff[1024];
	printf("Copy in archive. file name - %s\n", file_name);
	if((file = open(file_name, O_RDONLY, S_IRUSR | S_IWUSR)) == -1)
	{
		printf("Error, can`t open destination file\n");
	}
	while((nread = read(file, &buff, sizeof(buff))) > 0)
	{
		write(arch, buff, nread);
		//printf("%s", buff);
	}
	close(file);
	return;
}
void copy_data(int arch, char *path)
{
	DIR *dp;
	struct dirent *cr_dir, *cr_file;
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open dir\n");
		return;
	}
	while(1)
	{
		cr_file = readdir(dp);
		if(cr_file == NULL)
			break;
		if(cr_file->d_type == 8) // если просто файл
		{
			char buff[1024];
			strcpy(buff, path);
			strcat(buff, "/");
			strcat(buff, cr_file->d_name);
			copy_file(arch, buff);
		}
	}
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open dir\n");
		return;
	}
	while(1)
	{
		cr_dir = readdir(dp);
		if(cr_dir == NULL)
		{
			printf("empty\n");
			return;
		}
		if(strcmp(cr_dir->d_name, ".") == 0 || strcmp(cr_dir->d_name, "..") == 0)
			continue;
		else if(cr_dir->d_type == 4) // если директория
		{	
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_dir->d_name);
			printf("dir - %s\n", path);
			//printf("%d\n", dir_desc.id);
			copy_data(arch, full_path);
		}
	}
}
void pack(char *file_names_arr[], int arg_cnt, int start_index)
{
	struct stat fl_stat;
	int arch;
	int file_cnt = 0; //count of struct
	if((arch = open("my_arch", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		printf("Error, can`t create archive file \n");
	write(arch, &file_cnt, sizeof(int));
	for(int i = start_index; i < arg_cnt; i++)
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISREG(fl_stat.st_mode))
		{
			printf("cycle - 1\n");
			handle_file(arch, &file_cnt, file_names_arr[i], file_names_arr[i], -1);
		}
		else continue;
	}
	for(int i = start_index; i < arg_cnt; i++)
	{
		lstat(file_names_arr[i], &fl_stat);
 		if(S_ISDIR(fl_stat.st_mode))
		{
			printf("cycle - 2\n");
			char path[100];
			strcpy(path, file_names_arr[i]);
			handle_dir(arch, &file_cnt, path, file_names_arr[i], -1);//составили и записали дискрипторы для дерикторий и файлов
		}else continue;
	}
	for(int i = start_index; i < arg_cnt; i++)
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISREG(fl_stat.st_mode))
		{
			printf("cycle - 3\n");
			copy_file(arch, file_names_arr[i]);
		}else continue;
	} 
	for(int i = start_index; i < arg_cnt; i++)
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISDIR(fl_stat.st_mode))
		{
			printf("cycle - 4\n");
			char path[100];
			strcpy(path, file_names_arr[i]);
			copy_data(arch, path);
		}else continue;
	}


	lseek(arch, SEEK_SET, 0);
	write(arch, &file_cnt, sizeof(int));
	close(arch);
}
void unpack_dir(struct file_descriptor *fl_desc) // можно передавать только название директории
{
	printf("creting dir - %s\n", fl_desc->fl_name);
	mkdir(fl_desc->fl_name, O_CREAT| O_TRUNC | S_IRUSR| S_IWUSR);
	fl_desc->pr_id = 30001;
	//chdir(fl_desc->fl_name);
}
void unpack_file(int arch, struct file_descriptor *fl_desc)
{
	int file;
	printf("creating file %s\n", fl_desc->fl_name);
	if((file = open(fl_desc->fl_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		printf("Error, can`t create destination file \n");
	char buff[fl_desc->fl_sz];
	read(arch, &buff, sizeof(buff));
	printf("pass\n");
	write(file, &buff, sizeof(buff));
	close(file);
	fl_desc->pr_id = 30001;
}
void unpack(char *file_name)
{
	int arch, file_cnt = 0, file;
	if((arch = open(file_name, O_RDONLY)) == -1)
	{
		printf("Error, can`t open archive file\n");
		return;
	}
	read(arch, &file_cnt, sizeof(int));
	printf("%d\n", file_cnt);
	struct file_descriptor fl_desc[file_cnt];
	for(int i = 0; i < file_cnt; i++)
	{
		read(arch, &fl_desc[i], sizeof(struct file_descriptor));
		printf("         file %s     \n", fl_desc[i].fl_name);
		printf("-----------------------------------------------------------\n");
		printf("file type - %d\n", fl_desc[i].is_reg);
		printf("file size - %ld\n", fl_desc[i].fl_sz);
		printf("file name - %s\n", fl_desc[i].fl_name);
		printf("file id - %d\n", fl_desc[i].id);
		printf("file parents id - %d\n", fl_desc[i].pr_id);
		printf("-----------------------------------------------------------\n");
	}
	int crt_pr_id = -1, min_value = 30000;
	for(int i = 0; i < file_cnt; i++)
	{
		for(int i = 0; i < file_cnt; i++)
			if(fl_desc[i].pr_id == crt_pr_id)
			{
				if(fl_desc[i].is_reg == 1)
					unpack_file(arch, &fl_desc[i]);
				else unpack_dir(&fl_desc[i]);
			}
		for(int i = 0; i < file_cnt; i++)
			if(fl_desc[i].pr_id < min_value)
				min_value = fl_desc[i].pr_id;
		if(min_value == 30000) // условие завершения цикла 
			break;
		crt_pr_id = min_value;
		printf("posmatrivaem file s pr_id %d\n", min_value);
		for(int i = 0; i < file_cnt; i++)
			if(fl_desc[i].id == crt_pr_id && fl_desc[i].is_reg == 0)
			{
				chdir(fl_desc[i].fl_name);
				break;
			}
		min_value = 30000;	
	}
	close(arch);
}
int main(int argc, char* argv[])
{\
	int opt = getopt(argc, argv, "pu:");
	switch(opt)
	{
		case 'p':
		{
			pack(argv, argc, optind);
			printf("Done\n");
			break;
		}
		case 'u':
		{
			unpack(optarg);
			break;
		}
		break;
	}
	return 1;
}