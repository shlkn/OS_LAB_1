_archivator: archivator.c _pack.o _unpack.o
	gcc -o archivator -fsanitize=address -Wall archivator.c pack.o unpack.o

_pack.o: pack.c pack.h
	gcc pack.c -c

_unpack.o: unpack.c unpack.h
	gcc unpack.c -c
