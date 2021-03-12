
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

void pack_creation(char *file_names_arr[], int lenght, int start_index);
void unpack(char *file_name);
struct file_discriptor
{
	long int fl_sz;
	char fl_name[50];
};

int main(int argc, char* argv[])
{
	int opt = getopt(argc, argv, "pu:");
	switch(opt)
	{
		case 'p':
		{
			pack_creation(argv, argc, optind);
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

void pack_creation(char *file_names_arr[], int arg_cnt, int start_index)
{
	int file_cnt = 0;
	struct stat fl_stat;
	struct file_discriptor fl_dscp;
	char buff[1024]; // buffer for file copy
	int arch; // archive file discriptor 
	int file;
	if((arch = open("my_arch", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		printf("Error, can`t create archive file \n");
	write(arch, &file_cnt, sizeof(int));
	for(int i = start_index; i < arg_cnt; i++)
	{
		lstat(file_names_arr[i], &fl_stat);
		if(S_ISREG(fl_stat.st_mode))
		{
			fl_dscp.fl_sz = fl_stat.st_size;   
			strcpy(fl_dscp.fl_name, file_names_arr[i]);
			write(arch, &fl_dscp, sizeof(struct file_discriptor));
		}
		else if(S_ISDIR(fl_stat.st_mode))
		{
			continue; //do something
		}
	} //записали дискриптор для каждого файла



	for(int i = start_index; i < arg_cnt; i++)
	{
		int nread;
		if((file = open(file_names_arr[i], O_RDONLY, S_IRUSR | S_IWUSR)) == -1)
			printf("Error, can`t open destination file\n");
		while((nread = read(file, &buff, sizeof(buff))) > 0)
		{
			write(arch, buff, nread);
			printf("%s", buff);
		}
		close(file);
		file_cnt++;
	}
	lseek(arch, SEEK_SET, 0);
	write(arch, &file_cnt, sizeof(int));
	close(arch);
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
	struct file_discriptor all_desc[file_cnt];
	for(int i = 0; i < file_cnt; i++)
	{
		read(arch, &all_desc[i], sizeof(struct file_discriptor));
		printf("file name - %s file size - %ld\n", all_desc[i].fl_name, all_desc[i].fl_sz);

	}
	for(int i = 0; i < file_cnt; i++) // создание и распаковка файлов 
	{
		if((file = open(all_desc[i].fl_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
			printf("Error, can`t create destination file \n");
		char buff[all_desc[i].fl_sz];
		read(arch, &buff, sizeof(buff));
		write(file, &buff, sizeof(buff));
		//need to perform dir
	}
}