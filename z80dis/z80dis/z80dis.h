//
//  z80dis.h
//  z80dis
//
//  Created by mark on Nov 2018.
//  Copyright © 2018 Garetech. All rights reserved.
//

#ifndef z80dis_h
#define z80dis_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>


typedef enum {
    HL = 0x00,
    IX = 0xdd,
    IY = 0xfd
} idx_e;


typedef enum {
    atnon  = 0, // not applicable
    atnn      , // 16bit literal
    atn       , // 8 bit literal
    atidxd    , // +d byte relative ie (ix+d)
    ate       , // byte PC relative ie djnz e
    atidx     , // IX or IY
    atgrp
} argtype_t;

typedef unsigned char  mem_t;
typedef unsigned char   op_t;
typedef unsigned   char   z8_t;
typedef signed   char   dd_t;
typedef unsigned short z16_t;

typedef const char     fmt_t;

typedef struct inst_s inst_t;
struct inst_s {
    op_t       op;
    fmt_t      *pfmt;
    fmt_t      *orig;
    int        len;
    argtype_t  argtype1;
    argtype_t  argtype2;
    inst_t     *group; // eg. &ed00[0]
};

int   diz80(mem_t *mem, z16_t zpc);
extern inst_t opsnormal[];
extern inst_t ddcb[];

#endif /* z80dis_h */
