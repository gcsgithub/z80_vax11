#include <stdlib.h>


int isdbg()
{
        char *lnam;

        lnam = getenv("Z80$DBG");

        if (lnam && *lnam=='Y') {
                return(1);
        }
        else {
                return(0);
        }
}
