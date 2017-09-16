//
//  z80dis.c
//  z80dis
//
//  Created by mark on 14/01/2016.
//  Copyright © 2016 com.panteltje. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>

#define MAX_LINE_LEN	1024

struct comm{
    const char *com1;
    const char *com2;
    int bytes;
    int type;
    int proc;
};

//#define T_SIZE	6

/* Might be self-modifying code. */
int f_smc = 0;

/* 8080 incompatible code */
int f_z80 = 0;
int a_zilog = 0;

/* program counter */
unsigned int pc=0;
int pass=0;

const char *rarg[]={"b","c","d","e","h","l","(hl)","a"};

/********opcodes for z80 **********/
const struct comm comtab[]={
    /*00*/{"nop","vv",1,0,0},
    /*01*/{"ld bc,","vv",3,2,0},
    /*02*/{"ld (bc),a","vv",1,0,0},
    /*03*/{"inc bc","vv",1,0,0},
    /*04*/{"inc b","vv",1,0,0},
    /*05*/{"dec b","vv",1,0,0},
    /*06*/{"ld b,","vv",2,1,0},
    /*07*/{"rlca","vv",1,0,0},
    /*08*/{"ex af,af'","vv",1,0,1},
    /*09*/{"add hl,bc","vv",1,0,0},
    /*0a*/{"ld a,(bc)","vv",1,0,0},
    /*0b*/{"dec bc","vv",1,0,0},
    /*0c*/{"inc c","vv",1,0,0},
    /*0d*/{"dec c","vv",1,0,0},
    /*0e*/{"ld c,","vv",2,1,0},
    /*0f*/{"rrca","vv",1,0,0},
    /*10*/{"djnz ","vv",2,3,1},
    /*11*/{"ld de,","vv",3,2,0},
    /*12*/{"ld (de),a","vv",1,0,0},
    /*13*/{"inc de","vv",1,0,0},
    /*14*/{"inc d","vv",1,0,0},
    /*15*/{"dec d","vv",1,0,0},
    /*16*/{"ld d,","vv",2,1,0},
    /*17*/{"rla","vv",1,0,0},
    /*18*/{"jr ","vv",2,3,1},
    /*19*/{"add hl,de","vv",1,0,0},
    /*1a*/{"ld a,(de)","vv",1,0,0},
    /*1b*/{"dec de","vv",1,0,0},
    /*1c*/{"inc e","vv",1,0,0},
    /*1d*/{"dec e","vv",1,0,0},
    /*1e*/{"ld e,","vv",2,1,0},
    /*1f*/{"rra","vv",1,0,0},
    /*20*/{"jr nz,","vv",2,3,1},
    /*21*/{"ld hl,","vv",3,2,0},
    /*22*/{"ld (","),hl",3,12,0},
    /*23*/{"inc hl","vv",1,0,0},
    /*24*/{"inc h","vv",1,0,0},
    /*25*/{"dec h","vv",1,0,0},
    /*26*/{"ld h,","vv",2,1,0},
    /*27*/{"daa","vv",1,0,0},
    /*28*/{"jr z,","vv",2,3,1},
    /*29*/{"add hl,hl","vv",1,0,0},
    /*2a*/{"ld hl,(",")",3,12,0},
    /*2b*/{"dec hl","vv",1,0,0},
    /*2c*/{"inc l","vv",1,0,0},
    /*2d*/{"dec l","vv",1,0,0},
    /*2e*/{"ld l,","vv",2,1,0},
    /*2f*/{"cpl","vv",1,0,0},
    /*30*/{"jr nc,","vv",2,3,1},
    /*31*/{"ld sp,","vv",3,2,0},
    /*32*/{"ld (","),a",3,12,0},
    /*33*/{"inc sp","vv",1,0,0},
    /*34*/{"inc (hl)","vv",1,0,0},
    /*35*/{"dec (hl)","vv",1,0,0},
    /*36*/{"ld (hl),","vv",2,1,0},
    /*37*/{"scf","vv",1,0,0},
    /*38*/{"jr c,","vv",2,3,1},
    /*39*/{"add hl,sp","vv",1,0,0},
    /*3a*/{"ld a,(",")",3,12,0},
    /*3b*/{"dec sp","vv",1,0,0},
    /*3c*/{"inc a","vv",1,0,0},
    /*3d*/{"dec a","vv",1,0,0},
    /*3e*/{"ld a,","vv",2,1,0},
    /*3f*/{"ccf","vv",1,0,0},
    /*40*/{"ld b,b","vv",1,0,0},
    /*41*/{"ld b,c","vv",1,0,0},
    /*42*/{"ld b,d","vv",1,0,0},
    /*43*/{"ld b,e","vv",1,0,0},
    /*44*/{"ld b,h","vv",1,0,0},
    /*45*/{"ld b,l","vv",1,0,0},
    /*46*/{"ld b,(hl)","vv",1,0,0},
    /*47*/{"ld b,a","vv",1,0,0},
    /*48*/{"ld c,b","vv",1,0,0},
    /*49*/{"ld c,c","vv",1,0,0},
    /*4a*/{"ld c,d","vv",1,0,0},
    /*4b*/{"ld c,e","vv",1,0,0},
    /*4c*/{"ld c,h","vv",1,0,0},
    /*4d*/{"ld c,l","vv",1,0,0},
    /*4e*/{"ld c,(hl)","vv",1,0,0},
    /*4f*/{"ld c,a","vv",1,0,0},
    /*50*/{"ld d,b","vv",1,0,0},
    /*51*/{"ld d,c","vv",1,0,0},
    /*52*/{"ld d,d","vv",1,0,0},
    /*53*/{"ld d,e","vv",1,0,0},
    /*54*/{"ld d,h","vv",1,0,0},
    /*55*/{"ld d,l","vv",1,0,0},
    /*56*/{"ld d,(hl)","vv",1,0,0},
    /*57*/{"ld d,a","vv",1,0,0},
    /*58*/{"ld e,b","vv",1,0,0},
    /*59*/{"ld e,c","vv",1,0,0},
    /*5a*/{"ld e,d","vv",1,0,0},
    /*5b*/{"ld e,e","vv",1,0,0},
    /*5c*/{"ld e,h","vv",1,0,0},
    /*5d*/{"ld e,l","vv",1,0,0},
    /*5e*/{"ld e,(hl)","vv",1,0,0},
    /*5f*/{"ld e,a","vv",1,0,0},
    /*60*/{"ld h,b","vv",1,0,0},
    /*61*/{"ld h,c","vv",1,0,0},
    /*62*/{"ld h,d","vv",1,0,0},
    /*63*/{"ld h,e","vv",1,0,0},
    /*64*/{"ld h,h","vv",1,0,0},
    /*65*/{"ld h,l","vv",1,0,0},
    /*66*/{"ld h,(hl)","vv",1,0,0},
    /*67*/{"ld h,a","vv",1,0,0},
    /*68*/{"ld l,b","vv",1,0,0},
    /*69*/{"ld l,c","vv",1,0,0},
    /*6a*/{"ld l,d","vv",1,0,0},
    /*6b*/{"ld l,e","vv",1,0,0},
    /*6c*/{"ld l,h","vv",1,0,0},
    /*6d*/{"ld l,l","vv",1,0,0},
    /*6e*/{"ld l,(hl)","vv",1,0,0},
    /*6f*/{"ld l,a","vv",1,0,0},
    /*70*/{"ld (hl),b","vv",1,0,0},
    /*71*/{"ld (hl),c","vv",1,0,0},
    /*72*/{"ld (hl),d","vv",1,0,0},
    /*73*/{"ld (hl),e","vv",1,0,0},
    /*74*/{"ld (hl),h","vv",1,0,0},
    /*75*/{"ld (hl),l","vv",1,0,0},
    /*76*/{"halt","vv",1,0,0},
    /*77*/{"ld (hl),a","vv",1,0,0},
    /*78*/{"ld a,b","vv",1,0,0},
    /*79*/{"ld a,c","vv",1,0,0},
    /*7a*/{"ld a,d","vv",1,0,0},
    /*7b*/{"ld a,e","vv",1,0,0},
    /*7c*/{"ld a,h","vv",1,0,0},
    /*7d*/{"ld a,l","vv",1,0,0},
    /*7e*/{"ld a,(hl)","vv",1,0,0},
    /*7f*/{"ld a,a","vv",1,0,0},
    /*80*/{"add a,b","vv",1,0,0},
    /*81*/{"add a,c","vv",1,0,0},
    /*82*/{"add a,d","vv",1,0,0},
    /*83*/{"add a,e","vv",1,0,0},
    /*84*/{"add a,h","vv",1,0,0},
    /*85*/{"add a,l","vv",1,0,0},
    /*86*/{"add a,(hl)","vv",1,0,0},
    /*87*/{"add a,a","vv",1,0,0},
    /*88*/{"adc a,b","vv",1,0,0},
    /*89*/{"adc a,c","vv",1,0,0},
    /*8a*/{"adc a,d","vv",1,0,0},
    /*8b*/{"adc a,e","vv",1,0,0},
    /*8c*/{"adc a,h","vv",1,0,0},
    /*8d*/{"adc a,l","vv",1,0,0},
    /*8e*/{"adc a,(hl)","vv",1,0,0},
    /*8f*/{"adc a,a","vv",1,0,0},
    /*90*/{"sub b","vv",1,0,0},
    /*91*/{"sub c","vv",1,0,0},
    /*92*/{"sub d","vv",1,0,0},
    /*93*/{"sub e","vv",1,0,0},
    /*94*/{"sub h","vv",1,0,0},
    /*95*/{"sub l","vv",1,0,0},
    /*96*/{"sub (hl)","vv",1,0,0},
    /*97*/{"sub a","vv",1,0,0},
    /*98*/{"sbc a,b","vv",1,0,0},
    /*99*/{"sbc a,c","vv",1,0,0},
    /*9a*/{"sbc a,d","vv",1,0,0},
    /*9b*/{"sbc a,e","vv",1,0,0},
    /*9c*/{"sbc a,h","vv",1,0,0},
    /*9d*/{"sbc a,l","vv",1,0,0},
    /*9e*/{"sbc a,(hl)","vv",1,0,0},
    /*9f*/{"sbc a,a","vv",1,0,0},
    /*a0*/{"and b","vv",1,0,0},
    /*a1*/{"and c","vv",1,0,0},
    /*a2*/{"and d","vv",1,0,0},
    /*a3*/{"and e","vv",1,0,0},
    /*a4*/{"and h","vv",1,0,0},
    /*a5*/{"and l","vv",1,0,0},
    /*a6*/{"and (hl)","vv",1,0,0},
    /*a7*/{"and a","vv",1,0,0},
    /*a8*/{"xor b","vv",1,0,0},
    /*a9*/{"xor c","vv",1,0,0},
    /*aa*/{"xor d","vv",1,0,0},
    /*ab*/{"xor e","vv",1,0,0},
    /*ac*/{"xor h","vv",1,0,0},
    /*ad*/{"xor l","vv",1,0,0},
    /*ae*/{"xor (hl)","vv",1,0,0},
    /*af*/{"xor a","vv",1,0,0},
    /*b0*/{"or b","vv",1,0,0},
    /*b1*/{"or c","vv",1,0,0},
    /*b2*/{"or d","vv",1,0,0},
    /*b3*/{"or e","vv",1,0,0},
    /*b4*/{"or h","vv",1,0,0},
    /*b5*/{"or l","vv",1,0,0},
    /*b6*/{"or (hl)","vv",1,0,0},
    /*b7*/{"or a","vv",1,0,0},
    /*b8*/{"cp b","vv",1,0,0},
    /*b9*/{"cp c","vv",1,0,0},
    /*ba*/{"cp d","vv",1,0,0},
    /*bb*/{"cp e","vv",1,0,0},
    /*bc*/{"cp h","vv",1,0,0},
    /*bd*/{"cp l","vv",1,0,0},
    /*be*/{"cp (hl)","vv",1,0,0},
    /*bf*/{"cp a","vv",1,0,0},
    /*c0*/{"ret nz","vv",1,0,0},
    /*c1*/{"pop bc","vv",1,0,0},
    /*c2*/{"jp nz,","vv",3,2,0},
    /*c3*/{"jp ","vv",3,2,0},
    /*c4*/{"call nz,","vv",3,2,0},
    /*c5*/{"push bc","vv",1,0,0},
    /*c6*/{"add a,","vv",2,1,0},
    /*c7*/{"rst 0","vv",1,0,0},
    /*c8*/{"ret z","vv",1,0,0},
    /*c9*/{"ret","vv",1,0,0},
    /*ca*/{"jp z,","vv",3,2,0},
    /*cb*/{"qs","vv",99,88,77},
    /*cc*/{"call z,","vv",3,2,0},
    /*cd*/{"call ","vv",3,2,0},
    /*ce*/{"adc a,","vv",2,1,0},
    /*cf*/{"rst 8","vv",1,0,0},
    /*d0*/{"ret nc","vv",1,0,0},
    /*d1*/{"pop de","vv",1,0,0},
    /*d2*/{"jp nc,","vv",3,2,0},
    /*d3*/{"out (","),a",2,11,0},
    /*d4*/{"call nc,","vv",3,2,0},
    /*d5*/{"push de","vv",1,0,0},
    /*d6*/{"sub ","vv",2,1,0},
    /*d7*/{"rst 10h","vv",1,0,0},
    /*d8*/{"ret c","vv",1,0,0},
    /*d9*/{"exx","vv",1,0,0},
    /*da*/{"jp c,","vv",3,2,0},
    /*db*/{"in a,(",")",2,11,0},
    /*dc*/{"call c,","vv",3,2,0},
    /*dd*/{"qs","vv",99,88,77},
    /*de*/{"sbc a,","vv",2,1,0},
    /*df*/{"rst 18h","vv",1,0,0},
    /*e0*/{"ret po","vv",1,0,0},
    /*e1*/{"pop hl","vv",1,0,0},
    /*e2*/{"jp po,","vv",3,2,0},
    /*e3*/{"ex (sp),hl","vv",1,0,0},
    /*e4*/{"call po,","vv",3,2,0},
    /*e5*/{"push hl","vv",1,0,0},
    /*e6*/{"and ","vv",2,1,0},
    /*e7*/{"rst 20h","vv",1,0,0},
    /*e8*/{"ret pe","vv",1,0,0},
    /*e9*/{"jp (hl)","vv",1,0,0},
    /*ea*/{"jp pe,","vv",3,2,0},
    /*eb*/{"ex de,hl","vv",1,0,0},
    /*ec*/{"call pe,","vv",3,2,0},
    /*ed*/{"qs","vv",99,88,77},
    /*ee*/{"xor ","vv",2,1,0},
    /*ef*/{"rst 28h","vv",1,0,0},
    /*f0*/{"ret p","vv",1,0,0},
    /*f1*/{"pop af","vv",1,0,0},
    /*f2*/{"jp p,","vv",3,2,0},
    /*f3*/{"di","vv",1,0,0},
    /*f4*/{"call p,","vv",3,2,0},
    /*f5*/{"push af","vv",1,0,0},
    /*f6*/{"or ","vv",2,1,0},
    /*f7*/{"rst 30h","vv",1,0,0},
    /*f8*/{"ret m","vv",1,0,0},
    /*f9*/{"ld sp,hl","vv",1,0,0},
    /*fa*/{"jp m,","vv",3,2,0},
    /*fb*/{"ei","vv",1,0,0},
    /*fc*/{"call m,","vv",3,2,0},
    /*fd*/{"qs","vv",99,88,77},
    /*fe*/{"cp ","vv",2,1,0},
    /*ff*/{"rst 38h","vv",1,0,0},
};

const char *ed00[]={
        /*0*/"bios_0_BOOT",
        /*1*/"bios_1_WBOOT",
        /*2*/"bios_2_CONST",
        /*3*/"bios_3_CONIN",
        /*4*/"bios_4_CONOUT",
        /*5*/"bios_5_LIST",
        /*6*/"bios_6_PUNCHAUXOUT",
        /*7*/"bios_7_READER",
        /*8*/"bios_8_HOME",
        /*9*/"bios_9_SELDSK",
        /*10*/"bios_10_SETTRK",
        /*11*/"bios_11_SETSEC",
        /*12*/"bios_12_SETDMA",
        /*13*/"bios_13_READ",
        /*14*/"bios_14_WRITE",
        /*15*/"bios_15_LISTST",
        /*16*/"bios_16_SECTRAN",
        /*17*/"bios_17_CONOST",
        /*18*/"bios_18_AUXIST",
        /*19*/"bios_19_AUXOST",
        /*20*/"bios_20_DEVTBL",
        /*21*/"bios_21_DEVINI",
        /*22*/"bios_22_DRVTBL",
        /*23*/"bios_23_MULTIO",
        /*24*/"bios_24_FLUSH",
        /*25*/"bios_25_MOVE",
        /*26*/"bios_26_TIME",
        /*27*/"bios_27_SELMEM",
        /*28*/"bios_28_SETBNK",
        /*29*/"bios_29_XMOVE",
        /*30*/"bios_30_USERF",
        /*31*/"bios_31_RESERV1",
        /*32*/"bios_32_RESERV2",
        /*33*/"BDOS_9_C_WRITESTR",
        /*34*/"bios_34_RESERV4",
        /*35*/"bios_35_RESERV5",
        /*36*/"bios_36_RESERV6",
        /*37*/"bios_37_RESERV7",
        /*38*/"bios_38_RESERV8",
        /*39*/"bios_39_RESERV9",
        /*40*/"bios_40_RESERV10",
        /*41*/"bios_41_RESERV11",
        /*42*/"bios_42_RESERV12",
        /*43*/"bios_43_RESERV13",
        /*44*/"bios_44_RESERV14",
        /*45*/"bios_45_RESERV15",
        /*46*/"bios_46_RESERV16",
        /*47*/"bios_47_RESERV17",
        /*48*/"bios_48_RESERV18",
        /*49*/"bios_49_RESERV19",
        /*50*/"bios_50_RESERV20",
        /*51*/"bios_51_RESERV21",
        /*52*/"bios_52_RESERV22",
        /*53*/"bios_53_RESERV23",
        /*54*/"bios_54_RESERV24",
        /*55*/"bios_55_RESERV25",
        /*56*/"bios_56_RESERV26",
        /*57*/"bios_57_RESERV27",
        /*58*/"bios_58_RESERV28",
        /*59*/"bios_59_RESERV29",
        /*60*/"bios_60_RESERV30",
        /*61*/"bios_61_RESERV31",
        /*62*/"bios_62_RESERV32",
        /*63*/"bios_63_RESERV33"
};

const char *ed40[]={
    /*40*/"in b,(c)",
    /*41*/"out (c),b",
    /*42*/"sbc hl,bc",
    /*43*/"ld (",
    /*44*/"neg",
    /*45*/"retn",
    /*46*/"im 0",
    /*47*/"ld i,a",
    /*48*/"in c,(c)",
    /*49*/"out (c),c",
    /*4a*/"adc hl,bc",
    /*4b*/"ld bc,(",
    /*4c*/"00",
    /*4d*/"reti",
    /*4e*/"00",
    /*4f*/"ld r,a",
};

const char *ed50[]={
    /*50*/"in d,(c)",
    /*51*/"out (c),d",
    /*52*/"sbc hl,de",
    /*53*/"ld (",
    /*54*/"00",
    /*55*/"00",
    /*56*/"im 1",
    /*57*/"ld a,i",
    /*58*/"in e,(c)",
    /*59*/"out (c),e",
    /*5a*/"adc hl,de",
    /*5b*/"ld de,(",
    /*5c*/"00",
    /*5d*/"00",
    /*5e*/"im 2",
    /*5f*/"ld a,r",
};

const char *ed60[]={
    /*60*/"in h,(c)",
    /*61*/"out (c),h",
    /*62*/"sbc hl,hl",
    /*63*/"00",
    /*64*/"00",
    /*65*/"00",
    /*66*/"00",
    /*67*/"rrd",
    /*68*/"in l,(c)",
    /*69*/"out (c),l",
    /*6a*/"adc hl,hl",
    /*6b*/"00",
    /*6c*/"00",
    /*6d*/"00",
    /*6e*/"00",
    /*6f*/"rld",
};

const char *ed70[]={
    /*70*/"00",
    /*71*/"00",
    /*72*/"sbc hl,sp",
    /*73*/"ld (",
    /*74*/"00",
    /*75*/"00",
    /*76*/"00",
    /*77*/"00",
    /*78*/"in a,(c)",
    /*79*/"out (c),a",
    /*7a*/"adc hl,sp",
    /*7b*/"ld sp,(",
    /*7c*/"00",
    /*7d*/"00",
    /*7e*/"00",
    /*7f*/"00",
};

const char *eda0[]={
    /*a0*/"ldi",
    /*a1*/"cpi",
    /*a2*/"ini",
    /*a3*/"outi",
    /*a4*/"00",
    /*a5*/"00",
    /*a6*/"00",
    /*a7*/"00",
    /*a8*/"ldd",
    /*a9*/"cpd",
    /*aa*/"ind",
    /*ab*/"outd",
    /*ac*/"00",
    /*ad*/"00",
    /*ae*/"00",
    /*af*/"00",
};

const char *edb0[]={
    /*b0*/"ldir",
    /*b1*/"cpir",
    /*b2*/"inir",
    /*b3*/"otir",
    /*b4*/"00",
    /*b5*/"00",
    /*b6*/"00",
    /*b7*/"00",
    /*b8*/"lddr",
    /*b9*/"cpdr",
    /*ba*/"indr",
    /*bb*/"otdr",
    /*bc*/"00",
    /*bd*/"00",
    /*be*/"00",
    /*bf*/"00",
};

const char *edc0[]={
    /*c0*/"BIOS-const",
    /*c1*/"BIOS-conin",
    /*c2*/"BIOS-conout",
    /*c3*/"BIOS-diskio3",
    /*c4*/"BIOS-diskio3",
    /*c5*/"BIOS-read_rdr",
    /*c6*/"BIOS-write_pun",
    /*c7*/"BIOS-write_lpt", 
    /*c8*/"BIOS-flash",
    /*c9*/"BIOS-OFFLOAD",
    /*ca*/"BIOS-BLOCKWRITE",
    /*cb*/"cb-unk",
    /*cc*/"cc-unk",
    /*cd*/"cd-unk",
    /*ce*/"ce-unk",
    /*cf*/"cf-unk",
};

const char *ede0[]={
    /*e0*/"e0-unk",
    /*e1*/"e1-unk",
    /*e2*/"e2-unk",
    /*e3*/"e3-unk",
    /*e4*/"e4-unk",
    /*e5*/"e5-unk",
    /*e6*/"e6-unk", 
    /*e7*/"e7-unk",
    /*e8*/"e8-unk",
    /*e9*/"e9-unk",
    /*ea*/"ea-unk",
    /*eb*/"eb-unk",
    /*ec*/"ec-unk",
    /*ed*/"brk",
    /*ee*/"dreg",
    /*ef*/"ef-unk",
};

int ckrange(unsigned char *t, int skip, char *str);
int ckrange_rel(unsigned char *t, int skip, int pc, char *str);
static int _diz80(unsigned char *t);
int diz80(unsigned char *t, unsigned int pc);

///////////////////////////
void disout(char *fmt,...)
///////////////////////////
{
    va_list args;
    
    if (!fmt) {
        fmt= (char *) "";        // just in case they pass a null pointer
    }
    va_start(args,fmt);
    vfprintf(stdout, fmt,args);
    va_end(args);
    fflush(stdout);
}

int diz80(unsigned char *t, unsigned int pc)
{
    int inslen;
    int idx;
    
    fprintf(stdout, "%04X: ", pc&0xffff);
    inslen = _diz80(&t[pc]);
    fprintf(stdout, "\t;");
    for (idx=0;idx<inslen;idx++) {
        fprintf(stdout, "%02X ", t[pc++]);
    }
    fprintf(stdout, "\n");
    return(pc);
}

/* Returns number of bytes used */
static int _diz80(unsigned char *t)
{
    char stri[80];
    char stro[80];
    int a,r,i;
    char bstr[MAX_LINE_LEN];
    char rstr[MAX_LINE_LEN];
    
    /* indent all instructions*/
    disout((char *) "\t");
    
    if(t[0] == 0xcb) {
        a=t[1];
        if(a==-1) {
            disout("defb %02xh", t[0]);
            return(1);
        }
        if(a < 8)				disout("rlc ");
        else if((a >= 0x08) && (a < 0x10))	disout("rrc ");
        else if((a >= 0x10) && (a < 0x18))	disout("rl ");
        else if((a >= 0x18) && (a < 0x20))	disout("rr ");
        else if((a >= 0x20) && (a < 0x28))	disout("sla ");
        else if((a >= 0x28) && (a < 0x30))	disout("sra ");
        /* undocumented instruction SLI (shift left with 1 insertion) */
        else if((a >= 0x30) && (a < 0x38))	disout("sli ");
        else if((a >= 0x38) && (a < 0x40))	disout("srl ");
        else if((a >= 0x40) && (a < 0x80))	disout("bit %d,",(a-0x40)/8);
        else if((a >= 0x80) && (a < 0xc0))	disout("res %d,",(a-0x80)/8);
        else if(a >= 0xc0)			disout("set %d,",(a-0xc0)/8);
        disout("%s",rarg[a%8]);
        f_z80=1;
        return(2);
    } else if(t[0] == 0xed) {
        a=t[1];
        if(a==-1) {
            disout("defb %02xh", t[0]);
            return(1);
        }
        if     ((a>=0x00)&&(a<0x40)&&(ed00[a-0x00][0]!='0')) i=0;
        else if((a>=0x40)&&(a<0x50)&&(ed40[a-0x40][0]!='0')) i=1;
        else if((a>=0x50)&&(a<0x60)&&(ed50[a-0x50][0]!='0')) i=2;
        else if((a>=0x60)&&(a<0x70)&&(ed60[a-0x60][0]!='0')) i=3;
        else if((a>=0x70)&&(a<0x80)&&(ed70[a-0x70][0]!='0')) i=4;
        else if((a>=0xa0)&&(a<0xb0)&&(eda0[a-0xa0][0]!='0')) i=5;
        else if((a>=0xb0)&&(a<0xc0)&&(edb0[a-0xb0][0]!='0')) i=6;
        else if((a>=0xc0)&&(a<0xd0)&&(edb0[a-0xc0][0]!='0')) i=7; /* MG-ED */
        else if((a>=0xe0)&&(a<0xf0)&&(ede0[a-0xe0][0]!='0')) i=8; /* MG-ED */
        //else if((a>=0xf0)&&(a<0xf0)&&(ede0[a-0xf0][0]!='0')) i=9; /* MG-ED */
        else {
            disout("defb edh");
            disout(";next byte illegal after ed");
            return(1);
        }
        r=ckrange(t, 2,bstr);
        //if(r==-1) {
        //  disout("defb");
            // MG2016
            //  for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
            //return(i);
            //}
        f_z80=1;
        switch(i) {
	    case 0: disout("%s",ed00[a-0x00]);
		break;
            case 1: disout("%s",ed40[a-0x40]);
                break;
            case 2: disout("%s",ed50[a-0x50]);
                break;
            case 3: disout("%s",ed60[a-0x60]);
                break;
            case 4: disout("%s",ed70[a-0x70]);
                break;
            case 5: disout("%s",eda0[a-0xa0]);
                break;
            case 6: disout("%s",edb0[a-0xb0]);
                break;
            case 7: disout("%s",edc0[a-0xc0]);
                break;
            case 8: disout("%s",ede0[a-0xe0]);
                break;
        }
        
        /*if arg not in prog range it is a constant*/
        if(a == 0x43)
        {
            disout("%s),bc",bstr);
            return(4);
        }
        else if(a == 0x53)
        {
            disout("%s),de",bstr);
            return(4);
        }
        else if(a == 0x73)
        {
            disout("%s),sp",bstr);
            return(4);
        }
        else if((a == 0x4b) || (a == 0x5b) || (a == 0x7b))
        {
            disout("%s)",bstr);
            return(4);
        }
        return(2);
    } else if((t[0] == 0xdd) || (t[0] == 0xfd)) {
        if(t[0]==0xfd) {
            strcpy(stri,"iy");
        } else {
            strcpy(stri,"ix");
        }
        //if(t[1]== 0xff) { //MG2016
        //  disout("defb %02xh", t[0]);
        //    return(1);
        //}
        r=ckrange(t, 2,bstr);
        f_z80=1;
        switch(t[1])
        {
            case 0x09:
                disout("add %s,bc",stri);
                return(2);
                break;
            case 0x19:
                disout("add %s,de",stri);
                return(2);
                break;
            case 0x21:
                if(r==-1) {
                //  disout("defb");
                    // MG2016
                    //  for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
                    //return(i);
                }
                disout("ld %s,%s",stri,bstr);
                return(4);
                break;
            case 0x22:
                if(r==-1) {
                    //disout("defb");
                    // MG2016
                    //  for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
                    // return(i);
                }
                disout("ld (%s),%s",bstr,stri);
                return(4);
                break;
            case 0x23:
                disout("inc %s",stri);
                return(2);
                break;
            case 0x29:
                disout("add %s,%s",stri,stri);
                return(2);
                break;
            case 0x2a:
                if(r==-1) {
                    //disout("defb");
                    // MG2016
                    //for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
                    //return(i);
                }
                disout("ld %s,(%s)",stri,bstr);
                return(4);
                break;
            case 0x2b:
                disout("dec %s",stri);
                return(2);
                break;
            case 0x39:
                disout("add %s,sp",stri);
                return(2);
                break;
            case 0xe1:
                disout("pop %s",stri);
                return(2);
                break;
            case 0xe3:
                disout("ex (sp),%s",stri);
                return(2);
                break;
            case 0xe5:
                disout("push %s",stri);
                return(2);
                break;
            case 0xe9:
                disout("jp (%s)",stri);
                return(2);
                break;
            case 0xf9:
                disout("ld sp,%s",stri);
                return(2);
                break;
        } /* end switch */
        /* now it must be an instruction of type (ix+dd) or (iy+dd) */
        // MG2016
        //if((t[2]==0xff)||(t[3]==0xff)) {
        //  disout("defb");
        //  for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
        //  return(i);
        //}
        
        if(t[2] < 128) {
            sprintf(stro,"(%s+%02xh)",stri,t[2]);
            /* stro is: (ix+dd) or (iy+dd) */
        } else {
            sprintf(stro,"(%s-%02xh)",stri,256 - t[2]);
        }
        
        if(t[1]==0xcb)
        {
            switch(t[3])
            {
                case 0x06:
                    disout("rlc ");
                    break;
                case 0x0e:
                    disout("rrc ");
                    break;
                case 0x16:
                    disout("rl ");
                    break;
                case 0x1e:
                    disout("rr ");
                    break;
                case 0x26:
                    disout("sla ");
                    break;
                case 0x2e:
                    disout("sra ");
                    break;
                case 0x3e:
                    disout("srl ");
                    break;
                case 0x46:
                    disout("bit 0,");
                    break;
                case 0x4e:
                    disout("bit 1,");
                    break;
                case 0x56:
                    disout("bit 2,");
                    break;
                case 0x5e:
                    disout("bit 3,");
                    break;
                case 0x66:
                    disout("bit 4,");
                    break;
                case 0x6e:
                    disout("bit 5,");
                    break;
                case 0x76:
                    disout("bit 6,");
                    break;
                case 0x7e:
                    disout("bit 7,");
                    break;
                case 0x86:
                    disout("res 0,");
                    break;
                case 0x8e:
                    disout("res 1,");
                    break;
                case 0x96:
                    disout("res 2,");
                    break;
                case 0x9e:
                    disout("res 3,");
                    break;
                case 0xa6:
                    disout("res 4,");
                    break;
                case 0xae:
                    disout("res 5,");
                    break;
                case 0xb6:
                    disout("res 6,");
                    break;
                case 0xbe:
                    disout("res 7,");
                    break;
                case 0xc6:
                    disout("set 0,");
                    break;
                case 0xce:
                    disout("set 1,");
                    break;
                case 0xd6:
                    disout("set 2,");
                    break;
                case 0xde:
                    disout("set 3,");
                    break;
                case 0xe6:
                    disout("set 4,");
                    break;
                case 0xee:
                    disout("set 5,");
                    break;
                case 0xf6:
                    disout("set 6,");
                    break;
                case 0xfe:
                    disout("set 7,");
                    break;
                default:
                    disout("defb %02xh,%02xh,%02xh,%02xh\t;illegal sequence"\
                       ,t[0],t[1],t[2],t[3]);
                    return(4);
            }/*end switch t[3]*/
            disout("%s",stro);
            return(4);
        }/*end dd cb or fd cb*/
        else switch(t[1])
        {
            case 0x34:
                disout("inc %s",stro);
                break;
            case 0x35:
                disout("dec %s",stro);
                break;
            case 0x36:
                disout("ld %s,%02xh",stro,t[3]);
                return(4);
                break;
            case 0x46:
                disout("ld b,%s",stro);
                break;
            case 0x4e:
                disout("ld c,%s",stro);
                break;
            case 0x56:
                disout("ld d,%s",stro);
                break;
            case 0x5e:
                disout("ld e,%s",stro);
                break;
            case 0x66:
                disout("ld h,%s",stro);
                break;
            case 0x6e:
                disout("ld l,%s",stro);
                break;
            case 0x70:
                disout("ld %s,b",stro);
                break;
            case 0x71:
                disout("ld %s,c",stro);
                break;
            case 0x72:
                disout("ld %s,d",stro);
                break;
            case 0x73:
                disout("ld %s,e",stro);
                break;
            case 0x74:
                disout("ld %s,h",stro);
                break;
            case 0x75:
                disout("ld %s,l",stro);
                break;
            case 0x77:
                disout("ld %s,a",stro);
                break;
            case 0x7e:
                disout("ld a,%s",stro);
                break;
            case 0x86:
                disout("add a,%s",stro);
                break;
            case 0x8e:
                disout("adc a,%s",stro);
                break;
            case 0x96:
                disout("sub %s",stro);
                break;
            case 0x9e:
                disout("sbc a,%s",stro);
                break;
            case 0xa6:
                disout("and %s",stro);
                break;
            case 0xae:
                disout("xor %s",stro);
                break;
            case 0xb6:
                disout("or %s",stro);
                break;
            case 0xbe:
                disout("cp %s",stro);
                break;
            default:
                disout("defb %02xh,%02xh,%02xh\t;illegal sequence"\
                   ,t[0],t[1],t[2]);
                return(3);
        }/*end switch t[1]*/
        return(3);
    }/*end 0xdd and 0xfd group*/
    r=ckrange(t, 1,bstr);
    switch(comtab[t[0]].type)
    {
        case 0:
            disout("%s",comtab[t[0]].com1);
            disout("\t");
            break;
        case 1:
            // MG2016
            //if(t[1]==0xff) {
            //    disout("defb %02xh", t[0]);
            //    return(1);
            //}
            disout("%s",comtab[t[0]].com1);
            disout("%02xh",t[1]);
            break;
        case 2:
            //if(r==-1) {
            //  disout("defb");
                // MG2016
                //  for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
                //  return(i);
                //}
            /*
             0000: 	di		;F3
             0001: 	ld a,80h	;3E 80
             0003: 	out (ffh),a	;D3 FF
             0005: 	ld sp,(lldb) print comtab[t[0]].com1
             (char *const) $0 = 0x0000000100003565 "ld sp,"
             (lldb) print bstr
             (char [1024]) $1 = "H\x13\x9ex\377
             (lldb)
             */
            disout("%s",comtab[t[0]].com1);
            disout("%s",bstr);
            break;
        case 3:
            // MG2016
            //if(t[1]==0xff) {
            //   disout("defb %02xh", t[0]);
            //   return(1);
            //}
            disout("%s",comtab[t[0]].com1);
            if(comtab[t[0]].bytes == 2) {
                /*jr and djnz instr. are 2 bytes type 3*/
                
                /* don't need to check return value because we checked
                 * t[1] above */
                ckrange_rel(t, 1,pc,rstr);
                disout("%s",rstr);
                
            } else {
                disout("%02xh",t[1]);/*not jr*/
            }
            break;
        case 11:
            // MG2016
            //if(t[1]==0xff) {
            //  disout("defb %02xh", t[0]);
            //  return(1);
            //}
            disout("%s",comtab[t[0]].com1);
            disout("%02xh%s",t[1],comtab[t[0]].com2);
            break;
        case 12:
            if(r==-1) {
                //disout("defb");
                // MG2016
                //for(i=0;t[i]!=0xff;i++) disout("%c%02xh", i?',':' ', t[i]);
                //return(i);
            }
            disout("%s",comtab[t[0]].com1);
            disout("%s%s",bstr,comtab[t[0]].com2);
            break;
    }
    a=comtab[t[0]].bytes;
    if(comtab[t[0]].proc==1) {
        f_z80=1;
    }
    return(a);
}


/* Returns:
 * 	-1	Not enough data available
 * 	0	Constant
 * 	1	Symbol 		
 */
int ckrange(unsigned char *t, int skip, char *str)
{
    unsigned short val;
    
    val=t[skip+1]<<8 | t[skip+0];
    
    sprintf(str, "%04xh", val);
    
    return 0;
}

/* Returns:
 * 	-1	Not enough data available
 * 	0	Constant
 * 	1	Symbol 		*/
int ckrange_rel(unsigned char *t, int skip, int pc, char *str)
{
    if(a_zilog) {
        /* old Zilog za.com */
        if(t[1]<128) {
            sprintf(str,"%d",t[1]+2);
        } else {
            sprintf(str,"%d",t[1]+2-256);
        }
    } else {
        /* zasm by Günter Woigk and
         * z80asm */
        if(t[1]<128) {
            sprintf(str,"$%+d",t[1]+2);
        } else {
            sprintf(str,"$%+d",t[1]+2-256);
        }
    }
    return(0);
}
