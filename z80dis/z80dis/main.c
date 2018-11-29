//
//  main.c
//  z80dis
//
//  Created by mark on 11/10/2018.
//  Copyright © 2018 Garetech. All rights reserved.
//

#include "z80dis.h"
#include "loadhex.h"


mem_t z80_memory[0xffff];

int main(int argc, char *argv[])
{
    int r0 = 0;
    char *fnm;
    
    z16_t pc = 0x100;
    
    fnm = "/Users/mark/emu/Zilog/Z80_8080/zout/xx.hex";
    loadhex(fnm, z80_memory);
    
    while (pc < 0x016A) {
        r0 = diz80(z80_memory, pc);
        pc += r0;
    }
    
    return(r0);
}
