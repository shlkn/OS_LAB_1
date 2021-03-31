#include "unpack.h"
#include "pack.h"

int main(int argc, char* argv[])
{
	if(argc <= 2)
	{
		printf("Error, too few arguments.\nRight call: %s -p/-u file_name1 file_name2...\n", argv[0]);
		return -1;
	}
	int opt = getopt(argc, argv, "pu:");
	switch(opt)
	{
		case 'p':
		{
			pack(argv, argc, optind);
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