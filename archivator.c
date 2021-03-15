#include "unpack.h"
#include "pack.h"

int main(int argc, char* argv[])
{
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