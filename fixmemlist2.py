#!/usr/bin/python3
'''
      echo "Usage: ${THIS} <memoryfile>"  >&2
        echo "   read memoryfile which is the result of DBG> exmaine z80_memory"
        echo "   labelit with mbasic.lst  WRKSPC values
        cat <<!

# grep EQU mbasic.lst | awk '{printf "%s:%s\n", $3,$4}' | grep ^[0-F][0-f][0-f][0-f] | sort -n > values.txt

Z80\Z80_MEMORY[0:65534]
    [0]-[255]:  00
    [256]:      0C3
    [257]:      00
    [258]:      20
    [259]-[8191]:       00
    [8192]:     0C3


 sed 's/://;s/\[//;s/\]//' mbasic_z80memory07.txt | awk '{printf "%04X: %5d: %02X %c\n", $1,$1,$2,$2}' > mbasic_z80memory07fix.txt

grep WRKSPC mbasic.lst | grep \.EQU | awk '{printf "%s=%s;%s\n", $4,$3,$0}' > WRKSPCequ.txt

'''

from platform import python_version
import os, sys, getopt, errno
from parse import *
    


##########
def dbg(str):
##########
    global DEBUG
    if DEBUG != 0:
        print(str)

############
def Usage():
############

    print( f"{sys.argv[0]}: [-d] [-h] [-i infile| --ifile=infile] [-V symtabfile] [-Z]")
    print( "  -Z  skip zero values")
    sys.exit(errno.EINVAL)


###############
def main(argv):
###############
    global DEBUG
    global prnZ

    infile = ""
    symtab = "/Users/mark/emu/Zilog/Z80_8080/zout/values.txt"
    
    try:
        opts, args = getopt.getopt(argv,"dhi:V:Z",["ifile="])
    except getopt.GetoptError:
           Usage()

    for opt, arg in opts:
          if opt == '-h':
             Usage()
          elif opt in ("-i", "--ifile"):
             infile = arg
          elif opt in ("-V"):
              symtab = arg
          elif opt in ("-d"):
              DEBUG += 1
          elif opt in ("-Z"):
              prnZ = 0
         
    dbg("Input file is \"{0}\" symtab is \"{1}\"".format(infile,symtab))
    dbg("prnZ: {0}".format(prnZ))

    lupval = readDict(symtab, ":")

    if DEBUG >  1 :
        for key in lupval.keys():
            print("key: \"{0}\" = value: \"{1}\"".format(key,lupval[key]))

    with open(infile, "r") as ifd:
        z80mem = {}
    
        curaddr=0x0000
        startaddr=0
        endaddr=0
        val=0
        
        for line in ifd:
            line = line.rstrip('\n')
            line = line.strip()

            if '-' in line:
                (startaddr, endaddr, valstr) = parse('[{}]-[{}]:{}',line)
            else:
                (startaddr, valstr) = parse('[{}]:{}',line)
                endaddr = startaddr

            val=int(valstr, 16)
            curaddr=int(startaddr)
            eaddr=int(endaddr)

            while(curaddr <= eaddr):
                key="{:04X}".format(curaddr)
                z80mem[key]=val
                curaddr+=1

        if 'FFFF' not in z80mem:
            z80mem['FFFF'] = 0


    for key in z80mem:
           val=z80mem[key]
           if key != "FFFF":
                addr=int('0x'+key,16)
                addrnext=addr+1
                val2=z80mem["{:04X}".format(addrnext)]
                val16="{:02X}{:02X}".format(val2,val)
                if val16 in lupval:
                   val16sym=lupval[val16]
                else:
                   val16sym=""
           else:
                val16=""
                val16sym=""
        
           valchar=chr(val)
           if not valchar.isprintable():
                valchar='.'

           sym=""
           if key in lupval:
                sym=lupval[key]

           if ( prnZ == 0):
               if (val != 0 ):
                    print("%s:%-9s:%02X:%s  %s %s" % (key,sym, val,valchar,val16,val16sym) )
           else:
               print("%s:%-9s:%02X:%s  %s %s" % (key,sym, val,valchar,val16,val16sym) )

           curaddr+=1
	

############################
def readDict(filename, sep):
############################
    with open(filename, "r") as f:
        dict = {}
        for line in f:
            line = line.rstrip('\n')
            values = line.split(sep)
            dict[values[0]] = values[1]
        return(dict)


if __name__ == "__main__":
   global DEBUG
   global prnZ
   DEBUG = 0
   prnZ  = 1
   main(sys.argv[1:])
