//
//  loadhex.c
//  z80dis
//
//  Created by mark on 27/11/2018.
//  Copyright © 2018 Garetech. All rights reserved.
//
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "loadhex.h"
int  readhex(FILE *fd, byte_t *mem64k);
int  loadhexrec(char *buf, byte_t *mem64k); // addr must point to 64KB
byte_t get_hex_byte(char **pptr);
word_t get_hex_word(char **pptr);
byte_t get_hex_digit(char **pptr);

////////////////////////////////////
int loadhex(char *fnm, byte_t *mem64k)
////////////////////////////////////
{
    FILE *hfd;
    int  sts;
    
    if ((hfd = fopen(fnm, "r")) == (FILE *) NULL) {
        sts=errno;
        fprintf(stderr, "Error opening intel hex file: \"%s\":%d\n", fnm, sts);
        return(sts);
    }
    else {
        sts = readhex(hfd, mem64k);
        fclose(hfd);
    }
    return(sts);
}

/////////////////////////////////////
int readhex(FILE *fd, byte_t *mem64k)
/////////////////////////////////////
{
    int sts=0;
    char  buf[8192];

    
    while( fgets(buf, sizeof(buf), fd) ) {
            sts = loadhexrec(buf, mem64k);
    }
    
    
    
    return(sts);
}

////////////////////////////////////////
int loadhexrec(char *buf, byte_t *mem64k) // addr must point to 64KB
////////////////////////////////////////
{
    /*
     Start code, one character, an ASCII colon ':'.
     Byte count, two hex digits, indicating the number of bytes (hex digit pairs) in the data field. The maximum byte count is 255 (0xFF). 16 (0x10) and 32 (0x20) are commonly used byte counts.
     Address, four hex digits, representing the 16-bit beginning memory address offset of the data. The physical address of the data is computed by adding this offset to a previously established base address, thus allowing memory addressing beyond the 64 kilobyte limit of 16-bit addresses. The base address, which defaults to zero, can be changed by various types of records. Base addresses and address offsets are always expressed as big endian values.
     Record type (see record types below), two hex digits, 00 to 05, defining the meaning of the data field.
     Data, a sequence of n bytes of data, represented by 2n hex digits. Some records omit this field (n equals zero). The meaning and interpretation of data bytes depends on the application.
     Checksum, two hex digits, a computed value that can be used to verify the record has no errors.
     */
    
    int    bytesloaded=0;
    char   *ptr;
    hextype_t hextype;
    word_t addr;
    int    hexlen;
    byte_t checksum=0;
    byte_t byte;
    
    
    ptr = buf;
    
    if (*ptr++ != ':') {
        return(0);
    }
    
    byte      = get_hex_byte(&ptr);
    checksum += byte;
    hexlen    = byte;
    
    // addr    = get_hex_word(&ptr);
    byte      = get_hex_byte(&ptr);
    checksum += byte;
    addr      = (byte << 8 );
    
    byte      = get_hex_byte(&ptr);
    checksum += byte;
    addr     |= byte;
    
    
    byte      = get_hex_byte(&ptr);
    hextype   = byte;

    
    switch(hextype) {
        case Intelhex_data:
            while(hexlen--) {
                byte = get_hex_byte(&ptr);
                checksum += byte;
            
                mem64k[addr++] = byte;
                bytesloaded++;
            }
            checksum = (~checksum + 1);
            byte = get_hex_byte(&ptr);
            checksum &= 0xff;
            
            if (checksum != byte) {
                fprintf(stderr, "Checksum missmatch got :%02X expected: %02X\n", checksum, byte);
            }
            break;
        case Intelhex_eof:
            return(-1);
            break;
            
        case IntelHex_Extended_SegAddr:
        case IntelHex_Start_SegAddr:
        case IntelHex_Extended_LinearAddr:
        case IntelHex_Start_LinearAddr:
        default:
            break;
    }
    
    return(bytesloaded);
}

//////////////////////////////
byte_t get_hex_byte(char **pptr)
//////////////////////////////
{
    byte_t nibble;
    byte_t rval=0;
    
    if (pptr) {
        nibble = get_hex_digit(pptr);
        rval = (nibble << 4);
        nibble = get_hex_digit(pptr);
        rval |= nibble;
    }
    return(rval);
}

//////////////////////////////
word_t get_hex_word(char **pptr)
//////////////////////////////
{
    word_t rval=0;
    byte_t b1, b2;
    
    b1 = get_hex_byte(pptr);
    rval = (b1 << 8 );
    b2 = get_hex_byte(pptr);
    rval |= b2;
    
    return(rval);
}

/////////////////////////////////
byte_t get_hex_digit(char **pptr)
/////////////////////////////////
{
    char *ptr;
    char c;
    byte_t  hexdigit = 0xff;
    if (pptr) {
        ptr = *pptr;
        if (ptr) {
            c = toupper(*ptr);
            if ((c>='0') && (c<='9')) {
                hexdigit = c-'0';
                ptr++;
            }
            else if ((c >='A') && (c <='F')) {
                hexdigit = c-'A'+10;
                ptr++;
            }
        }
        *pptr = ptr;
    }
    return(hexdigit);
}
