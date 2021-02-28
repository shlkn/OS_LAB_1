#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void printdir(char *dir, int depth, int out)
{
    DIR *dp;
    struct dirent *entry; 
    struct stat statbuf;
    char *buf;
    if((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            continue;
            //printf("%*s%s/\n", depth, "", entry->d_name);
            for(int i = 0; i < depth; i++)
            	strcat(buf, " ");
            strcat(buf, entry->d_name);
            write(out, buf, sizeof(buf));
            printdir(entry->d_name, depth+4, out);
        } else printf("%*s%s\n", depth , " ", entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

/*void cpy()
{
char buf[1024]
int in, out;
in = open("file.in", O_RDONLY);
out = open("file.out", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
while(*/

int main(int argc, char *argv)
{
	//int out;
	printf("%s\n", argv);
    printf("Directory scan of /home\n");
    int out = open("file.out", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    printdir("/home", 0, out);
    printf("done");
    return 1;
}



