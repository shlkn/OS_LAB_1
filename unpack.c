#include "unpack.h"

void unpack_dir(struct unpack_file_descriptor *fl_desc) // можно передавать только название директории
{
	mkdir(fl_desc->fl_name, O_CREAT| O_TRUNC | S_IRUSR| S_IWUSR);
	fl_desc->pr_id = 30001; // sets parents id that means that file is was processed
}

void unpack_file(int arch, struct unpack_file_descriptor *fl_desc)
{
	int file;
	if((file = open(fl_desc->fl_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) 
		printf("Error, can`t create destination file \n");
	char buff[fl_desc->fl_sz];
	read(arch, &buff, sizeof(buff));
	write(file, &buff, sizeof(buff));
	close(file);
	fl_desc->pr_id = 30001; // sets parents id that means that file is was processed
}

void unpack(char *file_name)
{
	int arch, file_cnt = 0, file;
	if((arch = open(file_name, O_RDONLY)) == -1)
	{
		printf("Error, can`t open archive file\n");
		exit(-1);
	}
	read(arch, &file_cnt, sizeof(int)); // reading count of struct in archive
	struct unpack_file_descriptor fl_desc[file_cnt];
	for(int i = 0; i < file_cnt; i++)
		read(arch, &fl_desc[i], sizeof(struct unpack_file_descriptor)); // reading struct itself
	int crt_pr_id = -1, min_value = 30000;
	while(1)
	{
		for(int i = 0; i < file_cnt; i++) // checking all files with parents id equal to curent
			if(fl_desc[i].pr_id == crt_pr_id)
			{
				if(fl_desc[i].is_reg) // if file is reg file - calls unpack for file, else unpack for dir
					unpack_file(arch, &fl_desc[i]);
				else unpack_dir(&fl_desc[i]);
			}
		for(int i = 0; i < file_cnt; i++)
			if(fl_desc[i].pr_id < min_value)
				min_value = fl_desc[i].pr_id; // finds minimal parents id except processed ones
		if(min_value == 30000) // cond of the end of cycle
			break;
		crt_pr_id = min_value; 
		for(int i = 0; i < file_cnt; i++)
			if(fl_desc[i].id == crt_pr_id && fl_desc[i].is_reg == 0)
			{
				chdir(fl_desc[i].fl_name); // go to lower level
				break;
			}
		min_value = 30000;	
	}
	close(arch);
	printf("Done\n");
}