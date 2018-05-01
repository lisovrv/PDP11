#include <stdio.h>
#include <stdlib.h>

#include "PDP11lib.h"

extern byte mem [64 * 1024];
extern word reg [8];
extern struct Flags flag;



struct Command commands [] =
{
	{	  0,	0177777,	"halt",		do_halt,	NO_PARAM		},
	{010000,	0170000,	"mov",		do_mov, 	HAS_SS | HAS_DD	},
	{060000, 	0170000,	"add",		do_add,		HAS_DD | HAS_SS	},
	{077000,	0177000,	"sob",		do_sob,		HAS_NN			},
	{005000, 	0177000,	"clr",		do_clr,		HAS_DD			},
	{0110000,	0170000,	"movb",		do_movb, 	HAS_SS | HAS_DD	},
    {001400, 	 0xFF00,	"beq", 		do_beq, 	HAS_XX			},
    {000400,     0xFF00,    "br",       do_br,      HAS_XX },
	{	  0,		  0,	"unknown",	do_unknown,	NO_PARAM		}
};


extern struct Mode  ss, dd, hh, nn;




void do_halt (Wtc curcmd)
{
	printf("\n");
	print_reg();
	exit(0);
}

void do_mov (Wtc curcmd)
{
	dd.res = ss.val;
	if(dd.space == REG)
	{
		reg[dd.ad]= dd.res;
	}
	else
	{
		w_write(dd.ad, dd.res);
	}
	printf("\n");
}

void do_add (Wtc curcmd)
{
	dd.res = (doubleword) dd.val + (doubleword) ss.val;
	if(dd.space == REG)
	{
		reg[dd.ad]= dd.res;
	}
	else
	{
		w_write(dd.ad, dd.res);
	}
	printf("\n");

	change_flag(curcmd);
}

void do_unknown (Wtc curcmd)
{
	printf("\n");
	exit(1);
}

void do_sob (Wtc curcmd)
{
	reg[nn.ad]--;
	if (reg[nn.ad] != 0)
	{
		reg[7] -= 2 * nn.val;
	}
	printf ("\n");
}

void do_clr (Wtc curcmd)
{
	dd.val = 0;
	printf ("\n");
}

word byte_to_word(byte b)
{

	word w;
	if (sign(b, 1) == 0) {
		w = 0;
		w |= b;
	} else {
		w = ~0xFF;
		w |= b;
	}
	return w;

}

void do_movb(Wtc curcmd)
{
	dd.res = ss.val;
	if (dd.space == REG)
	{
		reg[dd.ad] = byte_to_word(dd.res);

	}
	else
	{
		b_write(dd.ad, (byte)dd.res);
	}
    printf ("\n");
}

void do_br(Wtc curcmd)
{
	printf("\n");
}

void do_beq(Wtc curcmd)
{
	printf("\n");
}

void change_flag(Wtc curcmd)
{
    if (curcmd.B)
    {
        flag.N = (dd.res >> 7) & 1;
    }
    else
    {
         flag.N = (dd.res >> 15) & 1;
    }

    flag.Z = (dd.res == 0);

    if (curcmd.B)
    {
        flag.C = (dd.res >> 8) & 1;
    }
    else
    {
         flag.C = (dd.res >> 16) & 1;
    }
}



