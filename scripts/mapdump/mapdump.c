#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#define CASE_SENSITIVE  1

#ifndef PATH_MAX       /* POSIX, but not required */
#if defined(_MSC_VER)  /* Microsoft C */
#define PATH_MAX        _MAX_PATH
#endif
#endif

#if defined UNIX
/* UNIX void definition */
/* File/extension seperator */
#define VOID        void
#define FSEPX       '.'
#define LKDIRSEP    '/'
#define LKDIRSEPSTR "/"
#define OTHERSYSTEM
#else
/* DOS/WINDOWS void definition */
/* File/extension seperator */
#define VOID        void
#define FSEPX       '.'
#define LKDIRSEP    '\\'
#define LKDIRSEPSTR "\\"
#define OTHERSYSTEM
#endif
#ifdef SDK
    #define LKOBJEXT "o"
#else /* SDK */
    #define LKOBJEXT "rel"
#endif /* SDK */

/*
 * This file defines the format of the
 * relocatable binary file.
 */

#define NCPS    80              /* characters per symbol (JLH: change from 8) */
#define NDATA   16              /* actual data */
#define NINPUT  PATH_MAX        /* Input buffer size */
#define NHASH   64              /* Buckets in hash table */
#define HMASK   077             /* Hash mask */
#define NLPP    60              /* Lines per page */
#define NTXT    16              /* T values */

/*
 * Global symbol types.
 */
#define S_REF   1               /* referenced */
#define S_DEF   2               /* defined */

/*
 * Area type flags
 */
#define A_CON     0000          /* concatenate */
#define A_OVR     0004          /* overlay */
#define A_REL     0000          /* relocatable */
#define A_ABS     0010          /* absolute */
#define A_NOPAG   0000          /* non-paged */
#define A_PAG     0020          /* paged */

/* Additional flags for 8051 address spaces */
#define A_DATA    0000          /* data space (default)*/
#define A_CODE    0040          /* code space */
#define A_XDATA   0100          /* external data space */
#define A_BIT     0200          /* bit addressable space */

/* Additional flags for hc08 */
#define A_NOLOAD  0400          /* nonloadable */
#define A_LOAD    0000          /* loadable (default) */

/*
 *      General assembler address type
 */
typedef unsigned int Addr_T;

/*
 *      A structure area is created for each 'unique' data/code
 *      area definition found as the REL files are read.  The
 *      struct area contains the name of the area, a flag byte
 *      which contains the area attributes (REL/CON/OVR/ABS),
 *      an area subtype (not used in this assembler), and the
 *      area base address and total size which will be filled
 *      in at the end of the first pass through the REL files.
 *      As A directives are read from the REL files a linked
 *      list of unique area structures is created by placing a
 *      link to the new area structure in the previous area structure.
 */
struct  area
{
        struct  area    *a_ap;  /* Area link */
        struct  areax   *a_axp; /* Area extension link */
        Addr_T  a_addr;         /* Beginning address of area */
        Addr_T  a_size;         /* Total size of the area */
        Addr_T  a_unaloc;       /* Total number of unallocated bytes, for error reporting */
        char    a_type;         /* Area subtype */
        int     a_flag;         /* Flag byte */
        char    a_id[NCPS];     /* Name */
        char    *a_image;       /* Something for hc08/lkelf */
        char    *a_used;        /* Something for hc08/lkelf */
};

/*
 *      An areax structure is created for every A directive found
 *      while reading the REL files.  The struct areax contains a
 *      link to the 'unique' area structure referenced by the A
 *      directive and to the head structure this area segment is
 *      a part of.  The size of this area segment as read from the
 *      A directive is placed in the areax structure.  The beginning
 *      address of this segment will be filled in at the end of the
 *      first pass through the REL files.  As A directives are read
 *      from the REL files a linked list of areax structures is
 *      created for each unique area.  The final areax linked
 *      list has at its head the 'unique' area structure linked
 *      to the linked areax structures (one areax structure for
 *      each A directive for this area).
 */
struct  areax
{
        struct  areax   *a_axp; /* Area extension link */
        struct  area    *a_bap; /* Base area link */
        Addr_T  a_addr;         /* Beginning address of section */
        Addr_T  a_size;         /* Size of the area in section */
};

/*
 *      A sym structure is created for every unique symbol
 *      referenced/defined while reading the REL files.  The
 *      struct sym contains the symbol's name, a flag value
 *      (not used in this linker), a symbol type denoting
 *      referenced/defined, and an address which is loaded
 *      with the relative address within the area in which
 *      the symbol was defined.  The sym structure also
 *      contains a link to the area where the symbol was defined.
 *      The sym structures are linked into linked lists using
 *      the symbol link element.
 */
struct  sym
{
        struct  sym     *s_sp;  /* Symbol link */
        struct  areax   *s_axp; /* Symbol area link */
        char    s_type;         /* Symbol subtype */
        char    s_flag;         /* Flag byte */
        Addr_T  s_addr;         /* Address */
	Addr_T  s_size;
        char    *s_id;          /* Name: JLH change from [NCPS] */
};

FILE    *dfp = NULL;    /*      Dump output file handle
                         */
FILE    *mfp = NULL;    /*      The file handle mfp points to the
                         *      currently open file
                         */
/*
 *      A sym structure is created for every unique symbol
 *      referenced/defined while reading the REL files.  The
 *      struct sym contains the symbol's name, a flag value
 *      (not used in this linker), a symbol type denoting
 *      referenced/defined, and an address which is loaded
 *      with the relative address within the area in which
 *      the symbol was defined.  The sym structure also
 *      contains a link to the area where the symbol was defined.
 *      The sym structures are linked into linked lists using
 *      the symbol link element.
 *
 *      struct  sym
 *      {
 *              struct  sym     *s_sp;          Symbol link
 *              struct  areax   *s_axp;         Symbol area link
 *              char    s_type;                 Symbol subtype
 *              char    s_flag;                 Flag byte
 *              Addr_T  s_addr;                 Address
 *              char    *s_id;                  Name (JLH)
 *      };
 */
struct  sym *symhash[NHASH]; /* array of pointers to NHASH
                              * linked symbol lists
                              */
struct  sym     *sp;    /*      Pointer to the current
                         *      sym structure
                         */
struct  sym     *lsp;   /*      Pointer to the last
                         *      sym structure
                         */
struct  area    *areap; /*      The pointer to the first
                         *      area structure of a linked list
                         */
struct  area    *ap;    /*      Pointer to the current
                         *      area structure
                         */
struct  areax   *axp;   /*      Pointer to the current
                         *      areax structure
                         */
int     xflag;          /*      -x, listing radix flag
			 */
char    *ip;            /*      Pointer into the MAP file text line in ib[]
                         */
char    ib[NINPUT];     /*      MAP file text line
                         */

void dump_exit(int i)
{
	fprintf(stderr, "Error exit on %d\n", i);
	if (dfp != NULL) fclose(dfp);
	if (mfp != NULL) fclose(mfp);
	exit(i);
}

int hash(register char *p)
{
        register int h, n;
	
        h = 0;
        n = NCPS;
        while (*p && n--) {
#if     CASE_SENSITIVE
                h += *p++;
#else
                h += ccase[(unsigned char)(*p++)];
#endif
        }
        return (h&HMASK);
}

VOID *new(unsigned int n)
{
        register char *p;
	
        if ((p = (char *) calloc(n, 1)) == NULL) {
                fprintf(stderr, "Out of space!\n");
                dump_exit(1);
        }
        return (p);
}

/*
 * Allocate space for "str", copy str into new space
 * Return a pointer to the allocated name, or NULL if out of memory
 */
char *StoreString( char *str )
{
	/* To avoid wasting memory headers on small allocations, we
	/  allocate a big chunk and parcel it out as required.
	/  These static variables remember our hunk
	*/
#define STR_STORE_HUNK 2000
	static char *pNextFree = NULL;
	static int  bytesLeft = 0;
	
	int  length;
	char *pStoredString;
	
	length = strlen( str ) + 1;  /* what we need, including null */
	
	if (length > bytesLeft) {
		/* no space.  Allocate a new hunk.  We lose the pointer to any
		/  old hunk.  We don't care, as the names are never deleted.
		*/
		pNextFree = (char*)new( STR_STORE_HUNK );
		bytesLeft = STR_STORE_HUNK;
	}
	
	/* Copy the name and terminating null into the name store */
	pStoredString = pNextFree;
	memcpy( pStoredString, str, length );
	
	pNextFree += length;
	bytesLeft -= length;
	
	return pStoredString;
}

/*
 *      Character Type Definitions
 */
#define SPACE   0000
#define ETC     0000
#define LETTER  0001
#define DIGIT   0002
#define BINOP   0004
#define RAD2    0010
#define RAD8    0020
#define RAD10   0040
#define RAD16   0100
#define ILL     0200

#define DGT2    DIGIT|RAD16|RAD10|RAD8|RAD2
#define DGT8    DIGIT|RAD16|RAD10|RAD8
#define DGT10   DIGIT|RAD16|RAD10
#define LTR16   LETTER|RAD16

unsigned char   ctype[128] = {
/*NUL*/ ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,
/*BS*/  ILL,    SPACE,  ILL,    ILL,    SPACE,  ILL,    ILL,    ILL,
/*DLE*/ ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,
/*CAN*/ ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,    ILL,
/*SPC*/ SPACE,  ETC,    ETC,    ETC,    LETTER, BINOP,  BINOP,  ETC,
/*(*/   ETC,    ETC,    BINOP,  BINOP,  ETC,    BINOP,  LETTER, BINOP,
/*0*/   DGT2,   DGT2,   DGT8,   DGT8,   DGT8,   DGT8,   DGT8,   DGT8,
/*8*/   DGT10,  DGT10,  ETC,    ETC,    BINOP,  ETC,    BINOP,  ETC,
/*@*/   ETC,    LTR16,  LTR16,  LTR16,  LTR16,  LTR16,  LTR16,  LETTER,
/*H*/   LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER,
/*P*/   LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER,
/*X*/   LETTER, LETTER, LETTER, BINOP,  ETC,    ETC,    BINOP,  LETTER,
/*`*/   ETC,    LTR16,  LTR16,  LTR16,  LTR16,  LTR16,  LTR16,  LETTER,
/*h*/   LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER,
/*p*/   LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER, LETTER,
/*x*/   LETTER, LETTER, LETTER, ETC,    BINOP,  ETC,    ETC,    ETC
};

FILE *afile(char *fn, char *ft, int wf)
{
	FILE *fp;
	char fb[PATH_MAX];
	char *omode;
	int i;
	
	switch (wf) {
        case 0: omode = "r"; break;
        case 1: omode = "w"; break;
        case 2: omode = "a"; break;
        case 3: omode = "rb"; break;
        case 4: omode = "wb"; break;
        case 5: omode = "ab"; break;
        default: omode = "r"; break;
	}
	
	/*Look backward the name path and get rid of the extension, if any*/
	i=strlen(fn);
	for (; (fn[i]!=FSEPX)&&(fn[i]!=LKDIRSEP)&&(fn[i]!='/')&&(i>0); i--);
	if ((fn[i]==FSEPX) && strcmp(ft, "lnk")) {
		strncpy(fb, fn, i);
		fb[i]=0;
	} else {
		strcpy(fb, fn);
	}
	
	/*Add the extension*/
	if (fb[i] != FSEPX) {
		fb[i] = FSEPX;
		fb[i+1] = 0;
		strcat(fb, strlen(ft)?ft:LKOBJEXT);
	}
	
	fp = fopen(fb, omode);
	if (fp == NULL) {
		fprintf(stderr, "%s: cannot %s.\n", fb, (wf%3)==1?"create":"open");
		//lkerr++;
	}
	return (fp);
}

char get()
{
        register int c;
	
        if ((c = *ip) != 0)
                ++ip;
        return (c);
}

void unget(int c)
{
        if (c != 0)
                --ip;
}

char getnb()
{
        register int c;
	
        while ((c=get())==' ' || c=='\t')
                ;
        return (c);
}

void skip(register int c)
{
        if (c < 0)
                c = getnb();
        while (ctype[c=get()] & (LETTER|DIGIT)) { ; }
        unget(c);
}

void getid(char *id, register int c)
{
        register char *p;

        if (c < 0) {
                c = getnb();
        }
        p = id;
        do {
                if (p < &id[NCPS])
                        *p++ = c;
        } while (ctype[c=get()] & (LETTER|DIGIT));
        unget(c);
        while (p < &id[NCPS])
                *p++ = 0;
}

void chop_crlf(char *str)
{
        register int i;

        i = strlen(str);
        if (i >= 1 && str[i-1] == '\n') str[i-1] = 0;
        if (i >= 2 && str[i-2] == '\r') str[i-2] = 0;
}

int lex_getline()
{
        if (mfp == NULL || fgets(ib, sizeof ib, mfp) == NULL) {
		return (0);
        }
        chop_crlf(ib);
        return (1);
}

/* Used for qsort call in lstsym */
static int _cmpSymByAddr(const void *p1, const void *p2)
{
	struct sym **s1 = (struct sym **)(p1);
	struct sym **s2 = (struct sym **)(p2);
	int delta = ((*s1)->s_addr + (*s1)->s_axp->a_addr) -
                    ((*s2)->s_addr + (*s2)->s_axp->a_addr);
	/* Sort first by address, then by name. */
	if (delta) {
		return delta;
	}
	return strcmp((*s1)->s_id,(*s2)->s_id);
}

static int _cmpSymBySize(const void *p1, const void *p2)
{
	struct sym **s1 = (struct sym **)(p1);
	struct sym **s2 = (struct sym **)(p2);
	return ((*s2)->s_size) - ((*s1)->s_size);
}

void syminit(void)
{
        struct sym **spp;
        spp = &symhash[0];
        while (spp < &symhash[NHASH])
                *spp++ = NULL;
}

int symeq(register char *p1, register char *p2)
{
#if     CASE_SENSITIVE
	return (strncmp( p1, p2, NCPS ) == 0);
#else
	return (as_strncmpi( p1, p2, NCPS ) == 0);
#endif
}

struct sym *lkpsym(char *id, int f)
{
        register struct sym *sp;
        register int h;
	
        h = hash(id);
        sp = symhash[h];
        while (sp != NULL) {
                if (symeq(id, sp->s_id))
                        return (sp);
                sp = sp->s_sp;
        }
        if (f == 0)
                return (NULL);
        sp = (struct sym *) new (sizeof(struct sym));
        sp->s_sp = symhash[h];
        symhash[h] = sp;
        sp->s_id = StoreString( id );   /* JLH */
	sp->s_axp = axp;
	sp->s_size = -1;
        return (sp);
}

Addr_T symval(register struct sym *tsp)
{
        register Addr_T val;
        val = tsp->s_addr;
        if (tsp->s_axp) {
                val += tsp->s_axp->a_addr;
        }
        return(val);
}

void lkparea(char *id)
{
	register struct area *tap;
	register struct areax *taxp;
	
	ap = areap;
	axp = (struct areax *) new (sizeof(struct areax));
	axp->a_addr = -1; /* default: no address yet */
	while (ap) {
		if (symeq(id, ap->a_id)) {
			taxp = ap->a_axp;
			while (taxp->a_axp)
				taxp = taxp->a_axp;
			taxp->a_axp = axp;
			axp->a_bap = ap;
			return;
		}
		ap = ap->a_ap;
	}
	ap = (struct area *) new (sizeof(struct area));
	if (areap == NULL) {
		areap = ap;
	} else {
		tap = areap;
		while (tap->a_ap)
			tap = tap->a_ap;
		tap->a_ap = ap;
	}
	ap->a_axp = axp;
	axp->a_bap = ap;
	strncpy(ap->a_id, id, NCPS);
	ap->a_addr = 0;
}

/* NCPS != 8 */
void lstarea(struct area *xp)
{
	register struct areax *oxp;
	register int i;
	register char *ptr;
	int nmsym;
	Addr_T ai, aj;
	struct sym *sp;
	struct sym **p;
	
	putc('\n', dfp);
	
	/* Find number of symbols in area */
	nmsym = 0;
	oxp = xp->a_axp;
	while (oxp) {
		for (i=0; i<NHASH; i++) {
			sp = symhash[i];
			while (sp != NULL) {
				if (oxp == sp->s_axp)
					++nmsym;
				sp = sp->s_sp;
			}
		}
		oxp = oxp->a_axp;
	}
	if (nmsym == 0) {
		return;
	}
	
	fprintf(dfp, "Area                               ");
	fprintf(dfp, "Addr   Size   Decimal %s\n",
		(xp->a_flag & A_BIT)?"Bits ":"Bytes");/* JCF: For BIT print bits...*/
	fprintf(dfp, "--------------------------------   ");
	fprintf(dfp, "----   ----   ------- ----- ------------\n");
	/* Output Area Header */
	ptr = &xp->a_id[0];
	fprintf(dfp, "%-32s", ptr );    /* JLH: width matches --- above */
	ai = xp->a_addr;
	aj = xp->a_size;
	if (xflag == 0) {
		fprintf(dfp, "   %04X   %04X", ai, aj);
	} else if (xflag == 1) {
		fprintf(dfp, " %06o %06o", ai, aj);
	} else if (xflag == 2) {
		fprintf(dfp, "  %05u  %05u", ai, aj);
	}
	fprintf(dfp, " = %6u. %s ", aj,
		(xp->a_flag & A_BIT)?"bits ":"bytes"); /* JCF: For BIT print bits...*/
	
        /*
         * Allocate space for an array of pointers to symbols
         * and load array.
         */
	if ( (p = (struct sym **) malloc(nmsym*sizeof(struct sym *)))
		== NULL) {
		fprintf(dfp, "\nInsufficient space to build Map Segment.\n");
		return;
	}
	nmsym = 0;
	oxp = xp->a_axp;
	while (oxp) {
		for (i=0; i<NHASH; i++) {
			sp = symhash[i];
			while (sp != NULL) {
				if (oxp == sp->s_axp) {
					p[nmsym++] = sp;
				}
				sp = sp->s_sp;
			}
		}
		oxp = oxp->a_axp;
	}
	
	qsort(p, nmsym, sizeof(struct sym *), _cmpSymBySize);
	
	/* Symbol Table Output */
	i = 0;
	fprintf(dfp, "\n\n");
	fprintf(dfp, "      Value  Global\n");
	fprintf(dfp, "   --------  --------------------------------");
	while (i < nmsym) {
		fprintf(dfp, "\n");
		fprintf(dfp, "     ");
		
		sp = p[i];
		aj = sp->s_size;
		if (xflag == 0) {
			fprintf(dfp, "%04X    ", aj);
		} else if (xflag == 1) {
			fprintf(dfp, "%06o  ", aj);
		} else if (xflag == 2) {
			fprintf(dfp, "%05u   ", aj);
		}
		ptr = &sp->s_id[0];
		fprintf(dfp, "%s", ptr );
		
		i++;
	}
	putc('\n', dfp);
	free(p);
}

void dump()
{
	ap = areap;
	while (ap) {
		if (strcmp(ap->a_id, "DSEG") == 0 ||
		    strcmp(ap->a_id, "ISEG") == 0 ||
		    strcmp(ap->a_id, "PSEG") == 0 ||
		    strcmp(ap->a_id, "XSEG") == 0 ||
		    strcmp(ap->a_id, "CSEG") == 0 ||
		    strcmp(ap->a_id, "SSEG") == 0 ||
		    strcmp(ap->a_id, "CONST") == 0) {
			lstarea(ap);
		}
		ap = ap->a_ap;
	}
}

#define STATE_BEGIN		0x00
#define STATE_RADIX		0x01
#define STATE_AREA_HEAD		0x02
#define STATE_AREA_SPLIT	0x03
#define STATE_AREA_CONT		0x04
#define STATE_SYM_HEAD		0x05
#define STATE_SYM_SPLIT		0x06
#define STATE_SYM_CONT		0x07
#define STATE_END		0x07

int state = STATE_BEGIN;
int memPage;

void exit_uneqs(char *str)
{
        char id[NCPS];
	getid(id, -1);
	if (strcmp(id, str))
		dump_exit(2);
}

void exit_uneqc(char ch)
{
        char id;
	id = getnb();
	if (ch != id)
		dump_exit(2);
}

void eval_size()
{
	if (lsp) {
		if (ap->a_flag & A_ABS)
			lsp->s_size = 0;
		else
			lsp->s_size = sp->s_addr - lsp->s_addr;
	}
}

void fixsymsize(struct area *xp)
{
	struct areax *oxp;
	register int i;
	struct sym *sp, *fsp = NULL, *lsp = NULL;

	oxp = xp->a_axp;
	while (oxp) {
		for (i=0; i<NHASH; i++) {
			sp = symhash[i];
			while (sp != NULL) {
				if (oxp == sp->s_axp) {
					if (!fsp || fsp->s_addr > sp->s_addr)
						fsp = sp;
					if (!lsp || lsp->s_addr < sp->s_addr)
						lsp = sp;
				}
				sp = sp->s_sp;
			}
		}
		oxp = oxp->a_axp;
	}
	if (fsp) {
		fsp->s_addr = xp->a_addr;
	}
	if (lsp) {
		lsp->s_size = xp->a_addr + xp->a_size - lsp->s_addr;
	}
}

void fixramsize()
{
	register struct area *ap;
	struct area *dseg = NULL, *iseg = NULL;
	struct area *oseg = NULL, *sseg = NULL;
	struct area *pseg = NULL, *xseg = NULL;
	
	/* determine DSEG size */
	for (ap = areap; ap; ap=ap->a_ap) {
		if ((ap->a_size != 0)) {
			if (!strcmp(ap->a_id, "DSEG"))
				dseg = ap;
			if (!strcmp(ap->a_id, "OSEG"))
				oseg = ap;
			if (!strcmp(ap->a_id, "ISEG"))
				iseg = ap;
			if (!strcmp(ap->a_id, "SSEG"))
				sseg = ap;
			if (!strcmp(ap->a_id, "PSEG"))
				pseg = ap;
			if (!strcmp(ap->a_id, "XSEG"))
				xseg = ap;
		}
	}

	assert(dseg && sseg);
	ap = oseg ? oseg : (iseg ? iseg : sseg);
	dseg->a_size = ap->a_addr - dseg->a_addr;
	fixsymsize(dseg);

	if (iseg) {
		oseg ? ap = oseg : dseg;
		iseg->a_addr = ap->a_addr + ap->a_size;
		fixsymsize(iseg);
	}

	if (xseg) {
		if (xseg->a_addr < 0x100)
			xseg->a_addr = 0x100;
		xseg->a_size = xseg->a_size - xseg->a_addr;
		fixsymsize(xseg);
	}
}

int parse()
{
        char id[NCPS];
	char c;
	Addr_T ai;
	
	if (*ip == '\0') return 1;
	
	switch (state) {
	case STATE_BEGIN:
	        getid(id, -1);
		if (strcmp(id, "Hexadecimal") == 0)
			xflag = 0;
		else if (strcmp(id, "Octal") == 0)
			xflag = 1;
		else if (strcmp(id, "Decimal") == 0)
			xflag = 2;
		else
			dump_exit(2);
		state = STATE_AREA_HEAD;
		break;
	case STATE_AREA_HEAD:
		lkparea("__unknown__");
		exit_uneqs("Area");
		exit_uneqs("Addr");
		exit_uneqs("Size");
		exit_uneqs("Decimal");
	        getid(id, -1);
		if (strcmp(id, "Bytes") == 0)
			ap->a_flag &= ~A_BIT;
		if (strcmp(id, "Bits") == 0)
			ap->a_flag |= A_BIT;
		exit_uneqc('(');
		exit_uneqs("Attributes");
		exit_uneqc(')');
		state = STATE_AREA_SPLIT;
		break;
	case STATE_AREA_SPLIT:
		do {
			c = getnb();
			if (c != '-') {
				if (c == '\0')
					break;
				dump_exit(2);
			}
		} while (c == '-');
		state = STATE_AREA_CONT;
		break;
	case STATE_AREA_CONT:
	        getid(id, -1);
		strncpy(ap->a_id, id, NCPS);
		if (strcmp(id, ".") == 0) {
			getid(id, -1);
			strcat(ap->a_id, "  ");
			strcat(ap->a_id, id);
		}
	        getid(id, -1);
		if (xflag == 0)
			sscanf(id, "%04X", &ai);
		else if (xflag == 1)
			sscanf(id, "%06o", &ai);
		else if (xflag == 2)
			sscanf(id, "%05u", &ai);
		ap->a_addr = ai;
	        getid(id, -1);
		if (xflag == 0)
			sscanf(id, "%04X", &ai);
		else if (xflag == 1)
			sscanf(id, "%06o", &ai);
		else if (xflag == 2)
			sscanf(id, "%05u", &ai);
		ap->a_size = ai;
		exit_uneqc('=');
	        getid(id, -1);
		if (ap->a_flag & A_BIT)
			exit_uneqs("bits");
		else
			exit_uneqs("bytes");
		exit_uneqc('(');
		memPage = 0x00;
		do {
			c = getnb();
			if (c != ',')
				unget(c);
			if (c == ')') {
				getid(id, -1);
				break;
			}
		        getid(id, -1);
			if (strcmp(id, "ABS") == 0) {
				ap->a_flag |= A_ABS;
			} else if (strcmp(id, "REL") == 0) {
				ap->a_flag &= ~A_ABS;
			} else if (strcmp(id, "OVR") == 0) {
				ap->a_flag |= A_OVR;
			} else if (strcmp(id, "CON") == 0) {
				ap->a_flag &= ~A_OVR;
			} else if (strcmp(id, "PAG") == 0) {
				ap->a_flag |= A_PAG;
			} else if (strcmp(id, "CODE") == 0) {
				ap->a_flag |= A_CODE;
				memPage = 0x0C;
			} else if (strcmp(id, "XDATA") == 0) {
				ap->a_flag |= A_XDATA;
				memPage = 0x0D;
			} else if (strcmp(id, "BIT") == 0) {
				ap->a_flag |= A_BIT;
				memPage = 0x0B;
			}
		} while (1);
		if (!(ap->a_flag & A_PAG)) {
			exit_uneqc('\0');
		} else {
			/* swallow things */
			do {
				c = getnb();
				if (c == '\0')
					break;
			} while (1);
		}
		state = STATE_SYM_HEAD;
		break;
	case STATE_SYM_HEAD:
		sp = lsp = NULL;
		exit_uneqs("Value");
		exit_uneqs("Global");
		state = STATE_SYM_SPLIT;
		break;
	case STATE_SYM_SPLIT:
		do {
			c = getnb();
			if (c != '-') {
				if (c == '\0')
					break;
				dump_exit(2);
			}
		} while (c == '-');
		state = STATE_SYM_CONT;
		break;
	case STATE_SYM_CONT:
		c = getnb();
		if (c == '\f') {
			state = STATE_END;
			fixsymsize(ap);
			return 0;
		} else {
			unget(c);
		}
	        getid(id, -1);
		if (strcmp(id, "Hexadecimal") == 0) {
			xflag = 0;
			state = STATE_AREA_HEAD;
			fixsymsize(ap);
			break;
		} else if (strcmp(id, "Octal") == 0) {
			xflag = 1;
			state = STATE_AREA_HEAD;
			fixsymsize(ap);
			break;
		} else if (strcmp(id, "Decimal") == 0) {
			xflag = 2;
			state = STATE_AREA_HEAD;
			fixsymsize(ap);
			break;
		}
                if (memPage != 0) {
			exit_uneqc(':');
		        getid(id, -1);
		}
		if (xflag == 0) {
			if (strlen(id) == 8)
				sscanf(id, "%08X", &ai);
			else
				sscanf(id, "%04X", &ai);
		} else if (xflag == 1) {
			sscanf(id, "%06o", &ai);
		} else if (xflag == 2) {
			sscanf(id, "%05u", &ai);
		}
		getid(id, -1);
		sp = lkpsym(id, 1);
		sp->s_addr = ai;
		eval_size();
		lsp = sp;
		break;
	}
	if (*ip)
		printf("%s\n", ip);
	return 1;
}

int main(int argc, char **argv)
{
	char *fn = ".."LKDIRSEPSTR".."LKDIRSEPSTR"sdfirm.map";

	mfp = afile(fn, "map", 0);
	if (mfp == NULL) {
		dump_exit(1);
	}
	if (argc > 1 && strcmp(argv[1], "stdout") == 0)
		dfp = stdout;
	else
		dfp = afile(fn, "usg", 1);
	if (dfp == NULL) {
		dump_exit(1);
	}

	syminit();

        while (lex_getline()) {
		ip = ib;
		
		if (!parse()) break;
        }
	fixramsize();
	dump();
	return 0;
}
