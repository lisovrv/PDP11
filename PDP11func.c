#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "enum.h"
#include "PDP11lib.h"

byte mem [64 * 1024];
word reg [8];
struct Flags flag;

extern struct Command commands[];

struct Mode  ss, dd, hh, nn;

word w_read (adr a)
{
    assert(a % 2 == 0);

    word wrd1= mem[a];
    word wrd2 = mem[a + 1];
    wrd2 <<= 8;
    return wrd1 + wrd2;
}

void w_write (adr a, word val)
{
    assert(a % 2 == 0);
    mem[a] = (byte) val & 0xFF;
    mem[a + 1] = (byte) (val >> 8) & 0xFF;

}

byte b_read(adr a)
{
	return mem[a];
}

void b_write(adr a, byte val)
{
	mem[a] = val;
}


void get_nn (word w)
{
	nn.ad = (w >> 6) & 07;
	nn.val = w & 077;
	printf ("R%o , %o", nn.ad, pc - 2*nn.val);
}

Wtc create_command(word w)
{
	Wtc c;
	c.w = w;
	c.B = (w >> 15);
	c.command = (w >> 12) & 15;
	c.mode_r1 = (w >> 9) & 7;
	c.r1 = (w >> 6) & 7;
	c.mode_r2 = (w >> 3) & 7;
	c.r2 = w & 7;
	return c;
}


void print_reg ()
{
	int i = 0;
	printf("------------------Print registers:----------------\n");
	for (i = 0; i < 8; i ++)
	{
		printf("reg[%d] = %o\n", i, reg[i]);
	}
	printf("------------------++++----------------\n");
}

Mod get_mode (word r, word mode, word b)
{
	switch(mode)
	{
		case 0:
		{
			printf("R%o", r);
			hh.ad = r;
			hh.val = reg[r];
			hh.space = REG;
			break;
		}

		case 1:
		{
			printf ("@R%o", r);
			hh.val = reg[r];
			hh.ad = w_read ((adr) reg[r]);
			hh.space = MEM;
			break;
		}

		case 2:
		{
			if (r == 7 || r == 6 || b == 0)
			{
				printf ("#%o", w_read ((adr) reg[r]));
				hh.ad = reg[r];
				hh.val = w_read ((adr) reg[r]);
				hh.space = MEM;
				reg[r] += 2;
			}
			else
			{
				printf ("(R%o)+", r);
				hh.ad =  reg[r];
				hh.val = b_read ((adr) reg[r]);
				hh.space = MEM;
				reg[r] ++;
			}
			break;
        }

        case 3:
		{
			printf ("@#%o", w_read((adr) (reg[r])));
			if (r == 7 || r == 6 || b == 0)
			{
				hh.ad = w_read ((adr) reg[r]);
				hh.val = w_read ((adr) w_read ((adr) (reg[r])));
				hh.space = MEM;
				reg[r] += 2;
			}
			else
			{
				hh.ad = w_read ((adr) reg[r]);
				hh.val = b_read ((adr) w_read ((adr) (reg[r])));
				hh.space = MEM;
				reg[r] ++;
			}
			break;
		}
		case 4:
		{
			printf ("-(R%o)", r);
			if (r == 7 || r == 6 || b == 0)
			{
				reg[r] -= 2;
				hh.ad = reg[r];
				hh.val = w_read ((adr) reg[r]);
				hh.space = MEM;
				break;
			}
			else
			{
				reg[r] --;
				hh.ad = reg[r];
				hh.val = b_read ((adr) reg[r]);
				hh.space = MEM;
				break;
			}
		}
		case 5:
		{
			printf ("@-(R%o)", r);
			reg[r] -= 2;
			hh.ad = w_read ((adr) reg[r]);
			hh.val = w_read ((adr) w_read ((adr) (reg[r])));
			hh.space = MEM;
			break;
		}
	}
	return hh;
}

void load_file(char * file)
{
	int size = strlen(file);
	if(size == 0)
    {
        printf("File is empty, ERROR!");
        exit(0);
    }

	unsigned int a, n, val;
	int i = 0;
	FILE * f = fopen(file, "r");
	if (f == NULL) {
		perror(file);
		exit(1);
	}
	while(fscanf(f, "%x%x", &a, &n) == 2)
	{
		for(i = a; i < (a + n); i++)
		{
			fscanf(f, "%x", &val);
			b_write(i, val);
		}
	}
	fclose(f);
}

void mem_dump(adr start, word n)
{
	assert((start % 2) == 0);
	int i;
	for(i = 0; i < n; i += 2)
	{
		printf("%06o : %06o\n", start + i, w_read(start + i));
	}
}

void run(adr pc0)
{
	int i = 0;
	pc = pc0;
	while(1)
	{
		word w = w_read(pc);
		pc += 2;
		Wtc curcmd = create_command(w);
		for(i = 0; ; i ++)
		{
			struct Command cmd = commands[i];
			if((w & cmd.mask) == cmd.opcode)
			{
				printf("%06o : %06o\t\t", pc - 2, w);
				printf("%s ", cmd.name);
				if (cmd.param & HAS_SS)
				{
					ss = get_mode(curcmd.r1, curcmd.mode_r1, curcmd.B);
					printf (", ");
				}
				if (cmd.param & HAS_DD)
				{
					dd = get_mode(curcmd.r2, curcmd.mode_r2, curcmd.B);
				}
				if (cmd.param & HAS_NN)
				{
					get_nn(w);
				}
				cmd.func(curcmd);
				break;
			}
		}
	}
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
    printf("func_testmem: %04x =  %02x%02x\n", w, b1, b0);

    w = 0x0c0d;
    w_write (4, w);
    b0 = b_read(4);
    b1 = b_read(5);
    printf("func_testmem: %04x =  %02x%02x\n", w, b1, b0);

    assert( b1 == 0x0c);
    assert( b0 == 0x0d);
}

union A
{
    unsigned short int a;
    unsigned int z;
};


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
