#ifndef PDP11
#define PDP11

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define POISON 1000
#define LO(x) ((x)&0xFF)
#define HI(x) (((x)>>8)&0xFF)
#define pc reg[7]
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD (1 << 1)
#define HAS_XX (1 << 2)
#define HAS_NN (1 << 3)

#define src_mode(w) (((w)&007000)>>9)
#define dst_mode(w) (((w)&000070)>>3)
#define src_reg(w) (((w)&000700)>>6)
#define dst_reg(w) ((w)&000007)



typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;

byte mem[64*1024];
word reg[8];
word nn;

void testmem();
byte b_read (adr a);
void b_write (adr a, byte val);
word w_read (adr a);
void w_write (adr a, word val);
void printWord(unsigned short int a);
void printByte(unsigned char a);
void load_file(char* file);
void mem_dump(adr start, word n);
void run(adr pc0);
void do_halt(word w);
void do_add(word w);
void do_mov(word w);
void do_sob(word w);
void do_unknown(word w);

word get_nn(word w);
word get_ss(word w);
word get_dd(word w);
word get_xx(word w);


struct Command{
    word opcode;
    word mask;
    char* name;
    void (* func)(word w);
    byte param;

} commands[] = {
        {0, 0177777, "halt" , do_halt, NO_PARAM},//0xFFFF
        {0010000, 0170000, "mov", do_mov, HAS_SS | HAS_DD},
        {0060000, 0170000, "add", do_add, HAS_NN},
        {0700000, 0177000, "sob", do_sob, HAS_NN},
        {0, 0, "unknown", do_unknown, HAS_NN}
};

struct Argument {
    adr a;
    word val;
} ss, dd;

/*class Pdp11
{
public:
}; */

#include "pdp11func.cpp"


int main (int argc, char** argv)
{
    testmem();

    load_file(argv[1]);
    run(0040);

    testmem();
    mem_dump( 0x40, 4);

    return 0;
}

#endif