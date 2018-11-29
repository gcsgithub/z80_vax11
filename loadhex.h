//
//  loadhex.h
//  z80dis
//
//  Created by mark on 27/11/2018.
//  Copyright © 2018 Garetech. All rights reserved.
//

#ifndef loadhex_h
#define loadhex_h

#include <stdio.h>

typedef unsigned char byte_t;
typedef unsigned short word_t;

typedef enum {
    Intelhex_data                = 0x00,
    Intelhex_eof                 = 0x01,
    IntelHex_Extended_SegAddr    = 0x02,
    IntelHex_Start_SegAddr       = 0x03,
    IntelHex_Extended_LinearAddr = 0x04,
    IntelHex_Start_LinearAddr    = 0x05
} hextype_t;

int loadhex(char *fnm, byte_t *addr);
#endif /* loadhex_h */
