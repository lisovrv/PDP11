#include <stdio.h>
#include <assert.h>


typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;


#define LO(x) ((x)&0xFF)
#define HI(x) (((x)>>8)&0xFF)

byte mem[64*1024];

/*union A
{
    unsigned short int a;
    unsigned int z;
}; */

void testmem();
byte b_read (adr a);
void b_write (adr a, byte val);
word w_read (adr a);
void w_write (adr a, word val);
void printWord(unsigned short int a);
void printByte(unsigned char a);
void load_file();
void mem_dump(adr start, word n);

byte b_read (adr a)
{
    return mem[a];
}

void b_write (adr a, byte val)
{
    mem[a] = val;
}

word w_read (adr a)
{
    assert(a % 2 == 0);

    word wrd1= mem[a];
    word wrd2 = mem[a + 1];
    //printWord(wrd2);
    wrd2 <<= 8;
    //printWord(wrd1);
    //printWord(wrd2);

   // printWord(wrd1 + wrd2);

    return wrd1 + wrd2;
}

void w_write (adr a, word val)
{
    assert(a % 2 == 0);

    //val = 0x0b0a;
    // a = 2
    // mem[2] = 0x0a
    mem[a] = (byte) val & 0xFF;   // (byte)val;

    // mem[3] = 0x0b;
    mem[a + 1] = (byte) (val >> 8) & 0xFF;

}

void testmem()
{
    byte b0, b1;
    word w;
    b0 = 0x0a;
    b1 = 0x0b;
    b_write(2, b0);
    b_write(3, b1);
    w = w_read(2);
    printf("%04x =  %02x%02x\n", w, b1, b0);

    w = 0x0c0d;
    w_write (4, w);
    b0 = b_read(4);
    b1 = b_read(5);
    printf("%04x =  %02x%02x\n", w, b1, b0);

    assert( b1 == 0x0c);
    assert( b0 == 0x0d);
}

void load_file()
{
    FILE * f = stdin;
    unsigned int  adr;
    unsigned int  n;
    unsigned int val;
    while(fscanf(f, "%x%x", &adr, &n) == 2)
    {

        //fscanf(f, "%x%x", &adr, &n);
        int i = 0;

        for(i = 0; i < n; i++)
        {
            fscanf(f, "%x", &val);
            b_write(adr + i, val);
        }
    }
}

void mem_dump(adr start, word n)
{
    assert(n % 2 == 0);
    int i = 0;
    for (i = 0; i < n; i += 2)
    {
        printf("%06o : %06o\n", start + i , w_read(start + i));
    }
}



int main()
{
    //printf("%d\n\n", sizeof(unsigned short int));

    testmem();
    load_file();
    mem_dump( 0x40, 4);

    return 0;
}

void printWord(unsigned short int a)
{
    int i;
    unsigned short int mask = 1;
    for ( i = 0; i < 16; i++)
    {
        unsigned short int msk = mask << (15 - i);
        printf("%u", (a & msk)  >> ( 15 - i));
    }
    printf("\n");
}

void printByte(unsigned char a)
{
    int i;
    unsigned char mask = 1;
    for ( i = 0; i < 8; i++)
    {
        unsigned char msk = mask << (7 - i);
        printf("%u", (a & msk)  >> ( 7 - i));
    }
    printf("\n");
}

