//
//  z80dis.c
//  z80dis
//
//  Created by mark on Nov 2018.
//  Copyright © 2018 Garetech. All rights reserved.
//

#include "z80dis.h"

char *fmtarg(char *argstr,mem_t *mem,z16_t iip,op_t op, inst_t *opinfo, int argc,idx_e idx,char * idxname,dd_t d);

//////////////////////////////////////
int   diz80(mem_t *mem, z16_t zpc)
//////////////////////////////////////
{
    // Returns instruction length
    inst_t *opinfo;
    inst_t *ops;
    op_t    op;
    mem_t   *pc;
    z16_t   iip;
    dd_t    d;
    z16_t   nn;
    z8_t    n;
    char    idxname[8];
    char    arg1[80];
    char    arg2[80];
    mem_t   prefix;
    idx_e   idx;
    char    outbuf[1024];
    char    hexdmp[256];
    char    *hexp;
    int     ii;
    
    idx    = HL;
    strcpy(idxname,"HL");
    ops    = opsnormal;
    prefix = 0x00;
    d      = 0;
    n      = 0;
    nn     = 0;
    
    iip    = zpc;
    pc     = &mem[iip++];
    op     = mem[zpc+0];
    opinfo = &ops[op];
    
    if (opinfo->group) {
        ops    = opinfo->group; // move to sub group
        prefix = op;
        op     = mem[zpc+1];
        opinfo = &ops[op];
        iip++;
        
        switch(prefix) {
            case 0xcb:
                break;
                
            case 0xed:
                break;
                
            case IX:
                strcpy(idxname,"IX");
                idx = IX;
                d = pc[2];
                break;
                
            case IY:
                strcpy(idxname,"IY");
                idx = IY;
                d = pc[2];
                break;
                
            case 0x00:
                break;
                
            default: // wtf should not happen
                break;
        }
    }

    if (op == 0xcb) {
        if (idx != HL) {  //DDCBddop
            d      = pc[2];
            op     = pc[3];
            ops    = ddcb;
            opinfo = &ops[op];
            iip    = zpc+opinfo->len; // next inst
        }
    }
    else if ((opinfo->argtype1 == atidxd) || (opinfo->argtype2 == atidxd)) {
        d = pc[2];
        iip = zpc+3;
    }

    arg1[0] = arg2[0] = '\0';
    if (opinfo->argtype1) {
        fmtarg(arg1, mem, iip, op, opinfo, 1, idx, idxname, d);
    }
    if (opinfo->argtype2) {
        fmtarg(arg2, mem, iip, op, opinfo, 2, idx, idxname, d);
    }
    
    if (arg1[0] && arg2[0]) {
        sprintf(outbuf, opinfo->pfmt, arg1, arg2);
    }
    else if (arg1[0]) {
        sprintf(outbuf, opinfo->pfmt, arg1);
    }
    else if (arg2[0]) {
        sprintf(outbuf, opinfo->pfmt, arg2);
    }
    else {
        strcpy(outbuf, opinfo->pfmt);
    }
    
    hexp = hexdmp;
    for (ii = 0 ; ii < opinfo->len; ii++) {
        sprintf(hexp, "%02X ", pc[ii]);
        hexp += 3;
        *hexp++ = ' ';
    }
    *hexp++ = '\0';
    fprintf(stdout, "%04X:\t%-24.24s\t; %s\n",zpc,  outbuf, hexdmp);
    return(opinfo->len);
}


///////////////////////////////////////////////////////////////////////////////
char *fmtarg(char *argstr,mem_t *mem,z16_t iip,op_t op, inst_t *opinfo, int argc,idx_e idx,char * idxname,dd_t d)
///////////////////////////////////////////////////////////////////////////////
{
    z8_t   byte, n;
    unsigned char e, iip_high, iip_low;
    int    se;
    z16_t  nn, taddr;
    argtype_t argtype;
    
    switch(argc) {
        case 1:
            argtype = opinfo->argtype1;
            break;
        case 2:
            argtype = opinfo->argtype2;
            break;
        default:
            argtype = atnon;
            argstr[0] = '\0';
            break;
    }
    
    byte = n = e = 0x00;
    nn   = 0x0000;
    
    switch(argtype) {
        case atnon: // not applicable
            argstr[0] = '0';
            break;
        case atnn:  // 16bit literal
            byte =  mem[iip++];
            nn   = ((mem[iip++] << 8) & 0xff00) | byte;
            sprintf(argstr, "%04Xh", nn);
            break;
        case atn:   // 8 bit literal
            n = mem[iip++] & 0xff ;
            sprintf(argstr, "%02Xh", n);
            break;
        case ate:    // byte PC relative ie djnz e
            e = mem[iip++];
            se = e;
            if(se>=128) {
                se -= 256;
            }
            taddr = iip + se;
            sprintf(argstr,"$%+d\t;%04Xh", se,taddr );
            break;
        case atidx:  // IX or IY
            sprintf(argstr,"%s", idxname);
            break;
        case atidxd: // (IX+d) or (IY+d)
            sprintf(argstr,"%s%+d", idxname, d);
            break;
        case atgrp:
            break;
    }
    return(argstr);
}
