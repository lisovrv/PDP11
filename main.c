#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned short int adr;
typedef unsigned char byte;

void load_file(char * file);
void run(adr pc0, char** argv);


extern byte mem [64 * 1024];


int main (int argc, char** argv)
{
    mem[0177564] |= 128;
    fprintf(stderr, "PDP is ready to act!\n");

    load_file(argv[1]);
    run(01000, argv);


    return 0;
}

