#include <stdio.h>
#include <stdlib.h>

#include "PDP11lib.h"

extern byte mem [64 * 1024];
extern word reg [8];
extern struct Flags flag;

extern struct sign xx;

extern struct Mode  ss, dd, hh, nn;

struct Command commands [] =
{
	{	  0,	0177777,	"halt",		do_halt,	NO_PARAM		},
	{010000,	0170000,	"mov",		do_mov, 	HAS_SS | HAS_DD	},
	{060000, 	0170000,	"add",		do_add,		HAS_DD | HAS_SS	},
	{077000,	0177000,	"sob",		do_sob,		HAS_NN			},
	{005000, 	0177700,	"clr",		do_clr,		HAS_DD			},
	{0110000,	0170000,	"movb",		do_movb, 	HAS_SS | HAS_DD	},
    {001400, 	 0xFF00,	"beq", 		do_beq, 	HAS_XX			},
    {000400,     0xFF00,    "br",       do_br,      HAS_XX          },
    {0105700,	0177700,	"tstb",		do_tstb,	HAS_DD			},
    {0100000,    0xFF00,    "bpl",      do_bpl,     HAS_XX          },
    {004000, 	0177000, 	"jsr",		do_jsr,	 	HAS_DD | HAS_RR },
	{0000200,	0177770, 	"rts",		do_rts,		HAS_RR_END		},
	{0070000, 	0177000,    "mul",		do_mul,		HAS_SN | HAS_R_T},
	{0005300, 	0177700,	"dec",		do_dec,		HAS_DD			},
    {005700,    0177700,    "tst",      do_tst,     HAS_DD          },
	{	  0,		  0,	"unknown",	do_unknown,	NO_PARAM		}

};

void do_mul(Wtc curcmd)
{
	long int res = reg[curcmd.r1] * ss.val;
	reg[curcmd.r1] = res;
	change_flag(curcmd);
	flag.V = 0;
	if(0)
    {
        flag.C = 1;
    }
    else
    {
        flag.C = 0;
    }
	printf("\n");
}

void do_dec(Wtc curcmd)
{
	dd.val--;
	reg[dd.ad] --;
	if (dd.space == MEM)
	{
		w_write(dd.ad, dd.val);
	}
	else
	{
		reg[dd.ad] = dd.val;
	}
    change_flag(curcmd);
	if(reg[dd.ad] == 0100000)
	{
		flag.V = 1;
	}
	else
	{
		flag.V = 0;
	}
	printf("\n");

}

void do_jsr (Wtc curcmd)
{
    w_write(sp, reg[curcmd.r1]);
    sp -= 2;
    reg[curcmd.r1] = pc;
    pc = dd.ad;
    printf("\n");
}

word get_reg_number(word w)
{
    printf(" R%o", (w>>6) & 07);
    return (w>>6) & 07;
}


void do_rts(Wtc curcmd)
{
    pc = reg[curcmd.r2];
    sp += 2;
    reg[curcmd.r2] = w_read(sp);
    printf("\n");
}


void do_tst(Wtc curcmd)
{
	dd.res = dd.val;
	change_flag(curcmd);
	flag.V = 0;
	flag.C = 0;
	printf("\n");
}


void do_tstb(Wtc curcmd)
{
	dd.res = dd.val;
	change_flag(curcmd);
	flag.V = 0;
	flag.C = 0;
	printf("\n");
}


void do_bpl(Wtc curcmd)
{
	if(flag.N == 0)
	{
		do_br(curcmd);
	}
	else
	{
		printf("\n");
	}
}

void do_br(Wtc curcmd)
{
    pc += 2 * xx.val;
    printf("\n");
}


void do_beq(Wtc curcmd)
{
	if(flag.Z == 1)
	{
		do_br(curcmd);
	}
	else
    {
        printf("\n");
    }
}


void do_clr (Wtc curcmd)
{
	dd.val = 0;
	if(dd.space == REG)
	{
		reg[dd.ad] = 0;
	}
	else
	{
		w_write(dd.ad, dd.val);
	}
    printf ("\n");

    flag.N = 0;
    flag.Z = 1;
    flag.V = 0;
    flag.C = 0;

}

void do_sob (Wtc curcmd)
{
	reg[nn.ad]--;
	if (reg[nn.ad] != 0)
	{
		pc -= 2 * nn.val;
	}
	printf ("\n");
}

void do_unknown (Wtc curcmd)
{
	printf("\n");
	exit(1);
}



void do_halt (Wtc curcmd)
{
	printf("\n");
	print_reg_beuty();
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

	change_flag(curcmd);
	flag.V = 0;

	printf("\n");
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
    change_flag(curcmd);
	flag.V = 0;
}

void do_add (Wtc curcmd)
{
	dd.res = (doubleword) dd.val + (doubleword) ss.val;
	if(dd.space == REG)
	{
		reg[dd.ad] = dd.res;
	}
	else
	{
		w_write(dd.ad, dd.res);
	}
	printf("\n");

	change_flag(curcmd);
    if (curcmd.B)
    {
        flag.C = (dd.res >> 8) & 1;
    }
    else
    {
         flag.C = (dd.res >> 16) & 1;
    }
    if (curcmd.B)
    {
        word a = (~ (((dd.val >> 7)&1) ^ ((ss.val >> 7)&1)))&1;
        if(a)
        {
            flag.V = (((dd.res >> 7)&1) ^ ((ss.val >> 7)&1));
        }
    }
    else
    {
        word a = (~ (((dd.val >> 15)&1) ^ ((ss.val >> 15)&1)))&1;
        if(a)
        {
            flag.V = (((dd.res >> 15)&1) ^ ((ss.val >> 15)&1));
        }
    }
}


