#include <stdlib.h>
#include <string.h>

int isval(char *nam);
int isdbg();
int istrace();
int envval( char *str, int len , char *buf);

int envval( char *str, int len , char *buf)
{
	char *val;

	val = getenv(str);
	if (val) {
		strncpy(buf, val, len);
		return(len);
	}
	else {
		return(-1);
	}
}

int isdbg()
{
	return(isval("Z80$DBG"));
}

int istrace()
{
	return(isval("Z80$TRCLVL"));
}

int isval(char *nam)
{
        char *lnam;
	int c;

        lnam = getenv(nam);

        if (lnam) {
		c = *lnam;  
		switch(c) {
		case 'Y':
			return(1);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return(c-'0');
			break;
		default:
		        return(0);
                }
        }
        else {
                return(0);
        }
}
