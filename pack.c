#include "pack.h"

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
	write(arch, &fl_desc, sizeof(struct file_descriptor));
	return;
}

void handle_dir(int arch, int *file_cnt, char *path, char *dir_name, int pr_id)
{
	DIR *dp;
	struct dirent *cr_dir;
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
	dir_desc.id = *file_cnt; // writing file id
	dir_desc.pr_id = cr_pr_id; // writing parents id
	write(arch, &dir_desc, sizeof(struct file_descriptor));
	(*file_cnt)++;
	while((cr_dir = readdir(dp)) != NULL) // handle only files
		if(cr_dir->d_type == 8) // is regular file?
		{
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_dir->d_name);
			handle_file(arch, file_cnt, full_path, cr_dir->d_name, dir_desc.id);
		}
	closedir(dp);
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open directory\n");
		return;
	}
	while((cr_dir = readdir(dp)) != NULL) // handle only directory
	{
		if(strcmp(cr_dir->d_name, ".") == 0 || strcmp(cr_dir->d_name, "..") == 0)
			continue; 
		if(cr_dir->d_type == 4) // is directory?
		{	
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_dir->d_name);
			handle_dir(arch, file_cnt, full_path, cr_dir->d_name, dir_desc.id); // recusive call of this function
		}
	}
	closedir(dp);
}

void copy_file(int arch, char *file_name)
{
	int nread, file;
	char buff[1024];
	if((file = open(file_name, O_RDONLY, S_IRUSR | S_IWUSR)) == -1)
		printf("Error, can`t open destination file\n");
	while((nread = read(file, &buff, sizeof(buff))) > 0)
		write(arch, buff, nread);
	close(file);
	return;
}

void copy_data(int arch, char *path)
{
	DIR *dp;
	struct dirent *cr_dir;
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open directory\n");
		return;
	}
	while((cr_dir = readdir(dp)) != NULL) // handle only files
		if(cr_dir->d_type == 8) // is regular file?
		{
			char buff[1024];
			strcpy(buff, path);
			strcat(buff, "/");
			strcat(buff, cr_dir->d_name);
			copy_file(arch, buff);
		}
	closedir(dp);
	dp = opendir(path);
	if(dp == NULL) 
	{
		printf("Error can`t open dir\n");
		return;
	}
	while((cr_dir = readdir(dp)) != NULL) // handle only directory
	{
		if(strcmp(cr_dir->d_name, ".") == 0 || strcmp(cr_dir->d_name, "..") == 0)
			continue;
		else if(cr_dir->d_type == 4) // is directory?
		{	
			char full_path[100];
			strcpy(full_path, path);
			strcat(full_path, "/");
			strcat(full_path, cr_dir->d_name);
			copy_data(arch, full_path); // recusive call of this function
		}
	}
	closedir(dp);
}

void pack(char *file_names_arr[], int arg_cnt, int start_index)
{
	struct stat fl_stat;
	int arch;
	int file_cnt = 0; //count of struct
	if((arch = open("my_arch", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) // open or create file for archive
		printf("Error, can`t create archive file \n");
	write(arch, &file_cnt, sizeof(int)); // writes in archive count of files. in the end function this variable will be rewritten
	for(int i = start_index; i < arg_cnt; i++) // cycle only for regular files
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISREG(fl_stat.st_mode))
			handle_file(arch, &file_cnt, file_names_arr[i], file_names_arr[i], -1);
		else continue;
	}
	for(int i = start_index; i < arg_cnt; i++) // cycle only for directory
	{
		lstat(file_names_arr[i], &fl_stat);
 		if(S_ISDIR(fl_stat.st_mode))
		{
			char path[100];
			strcpy(path, file_names_arr[i]);
			handle_dir(arch, &file_cnt, path, file_names_arr[i], -1);
		}else continue;
	}
	// made headers and wrote it in archive
	// starting to copy data in archive out of files
	for(int i = start_index; i < arg_cnt; i++) // cycle only for regular files
	{ 
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISREG(fl_stat.st_mode))
			copy_file(arch, file_names_arr[i]);
		else continue;
	} 
	for(int i = start_index; i < arg_cnt; i++) // cycle only for directory
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISDIR(fl_stat.st_mode))
		{
			char path[100];
			strcpy(path, file_names_arr[i]);
			copy_data(arch, path);
		}else continue;
	}
	lseek(arch, SEEK_SET, 0); // moves offset to start of the file
	write(arch, &file_cnt, sizeof(int)); // rewrites count of files in the start of archive
	close(arch); // close archive file
	printf("Done\n");
}