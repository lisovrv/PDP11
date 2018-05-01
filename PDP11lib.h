#ifndef PDP11LIB_H_INCLUDED
#define PDP11LIB_H_INCLUDED

typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;
typedef int doubleword;

extern byte mem [64 * 1024];
extern word reg [8];

#define pc reg[7]
#define REG 1
#define MEM 0
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD (1 << 1)
#define HAS_NN (1 << 2)
#define HAS_XX (1 << 3)


# define LO(x) ((x) & 0xFF)
# define HI(x) (((x) >> 8) & 0xFF)
#define sign(w, is_byte) (is_byte ? ((w)>>7)&1 : ((w)>>15)&1 )

word w_read  (adr a);
void w_write (adr a, word val);
byte b_read  (adr a);
void b_write (adr a, byte val);

struct Flags
{
    int N;
    int Z;
    int C;
};

extern struct Flags flag;

struct Wordtocmd
{
	word w;       // wrd
	byte B;        // byte
	word command; // opcode
	word mode_r1; //mode 1 operand
	word r1;      // 1 operand
	word mode_r2; // mode 2 operand
	word r2;      // 2 operand
};

typedef struct Wordtocmd Wtc;

struct Mode
{
	word ad;		// address
	word val;		// value
	word res;		// result
	word space; 	// address in mem[ ] or reg[ ]
};
extern struct Mode  ss, dd, hh, nn;

typedef struct Mode Mod;



void testmem();
void load_file (char* file);
void mem_dump (adr start, word n);
void print_reg ();
Wtc create_command (word w);
void run (adr pc0);

Mod get_mode (word r, word mode, word b);
void get_nn (word w);

void do_halt (Wtc curcmd);
void do_mov (Wtc curcmd);
void do_movb (Wtc curcmd);
void do_add (Wtc curcmd);
void do_unknown (Wtc curcmd);
void do_sob (Wtc curcmd);
void do_clr (Wtc curcmd);
word byte_to_word(byte b);
void do_movb(Wtc curcmd);
void do_beq(Wtc curcmd);
void do_br(Wtc curcmd);
void change_flag(Wtc curcmd);

struct Command
{
	word opcode;
	word mask;
	char * name;
	void (* func)(Wtc curcmd);
	byte param;
};
///#include "PDP11func.cpp"

#endif
