#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/** simple comment filter and line counter -- gjh 2004 **/

enum cstate { PLAIN, IN_STRING, IN_QUOTE, S_COMM, COMMENT, CPP_COMMENT, E_COMM };

static enum cstate state = PLAIN;
static char buf[8192];
static int column, nc_linecount, n_comments;
static int raw, relative, verbose, unix_style = 1;
static int numbered, showlines, lastadded;
static int semi_cnt, t_semi_cnt;
static int sum_rw = 0;
static int sum_nc = 0;
static int sum_cm = 0;
static int sum_sm = 0;
static int ignore_empty_files = 0;

static void
add_buf(int c)
{
	if (column < sizeof(buf))
		buf[column] = c;

	column++;
#if 0
	if (lastadded == '/' && c == '/'
	&&  column == 2) /* recognizes only trivial cases */
		nc_linecount--; /* C++ style comment */
#endif
	lastadded = c;
}

static void
semi_check(char *b)
{	char *s;
	unsigned char saw_semilast = 0;

	for (s = b; *s != '\0'; s++)
		switch (*s) {
		case ';':
		case ',':
			saw_semilast = 1;
			semi_cnt++;
			break;
		case ' ':
		case '\t':
		case '\f':
		case '\v':
		case '\r':
			break;
		default:
			saw_semilast = 0;
			break;
		}
	if (saw_semilast)
		t_semi_cnt++;	
}

static void
process_line(char *filename)
{	int i;

	if (state == PLAIN)
	{	if (column > 0)
		{	for (i = 0; i < column; i++)
				if (!isspace(buf[i]))
					break;

			if (i == column)
				goto emptyline;

			if (column >= sizeof(buf) && showlines)
				fprintf(stderr, "%s:%d, error: line too long\n",
					filename, relative);
			else
			{	buf[column] = '\0';
				if (verbose)
					semi_check(&buf[i]);
				if (showlines)
				{	if (numbered)
						printf("%6d: ", nc_linecount+1);
					printf("%s\n", buf);
			}	}
			nc_linecount++;
		} else
		{
emptyline:		if (showlines) printf("\n");
		}
	}
	column = lastadded = 0;
	raw++; relative++;
}

static void
process_file(FILE *fd, char *filename, ...)
{	int c, start_delim = 0;

	while ((c = getc(fd)) != EOF)
	{	if (c == '\r')
			continue;
#if 0
	if (column < sizeof(buf)
	{	buf[column] = '\0';
		fprintf(stderr, "<%c> state %d <%s>\n", c, state, buf);
	}
#endif
		switch (state) {
		case PLAIN:
			switch (c) {
			case  '"': state = IN_STRING; break;
			case '\'': state = IN_QUOTE; break;
			case  '/': state = S_COMM; start_delim = column; break;
			case '\\':
				c = getc(fd);
same:				add_buf('\\');
				break;
			}
			break;
		case IN_STRING:
			switch (c) {
			case '"': state = PLAIN; break;
			case '\\': c = getc(fd); goto same;
			}
			break;
		case IN_QUOTE:
			switch (c) {
			case '\'': state = PLAIN; break;
			case '\\': c = getc(fd); goto same;
			}
			break;
		case S_COMM:
			switch (c) {
			case '*': state = COMMENT;
				  n_comments++;
				  column = start_delim-1;
				  break;
			case '/': state = CPP_COMMENT;
				  n_comments++;
				  column = start_delim-1;
				  /* nc_linecount--; */
				  break;		/* C++ style comment */
			default:  state = PLAIN;
				  break;
			}
			break;
		case COMMENT:
			switch (c) {
			case '*':
				state = E_COMM;
				/* fall through */
			default: 
				break;
			}
			break;
		case E_COMM:
			switch (c) {
			case '/': state = PLAIN; continue;
			case '*': break;
			default:  state = COMMENT; break;
			}
			break;
		case CPP_COMMENT:
			if (c == '\n') state = PLAIN;
			break;
		}
		if (c == '\n')
			process_line(filename);
		else if (state != COMMENT && state != CPP_COMMENT && state != E_COMM)
			add_buf(c);
	}
	
}

int
main(int argc, char *argv[])
{	FILE *fd = stdin;
	int nrfiles = 0;

	while (argc > 1 && argv[1][0] == '-')
	{	switch (argv[1][1]) {
		case 'n':
			numbered = 1;
			/* fall through */
		case 's':
			showlines = 1;
			break;
		case 'u':
			unix_style = 1 - unix_style;
			break;
		case 'v':
			verbose++;
			break;
		case 'i':
		    ignore_empty_files++;
		    break;
		default:
			fprintf(stderr, "usage: ncsl [-n] [-s] [-m] *.c\n");
			fprintf(stderr, "by default, wc-like output with 3 or 4 fields\n");
			fprintf(stderr, "	field 1: nr of raw lines of input\n");
			fprintf(stderr, "	field 2: nr of non-blank/non-comment lines\n");
			fprintf(stderr, "	field 3: nr comments\n");
			fprintf(stderr, "	field 4: nr of semi-colons and commas\n");
			fprintf(stderr, "options:\n");
			fprintf(stderr, " -n show stripped output with numbered lines\n");
			fprintf(stderr, " -s show stripped output\n");
			fprintf(stderr, " -u show results one field per line, cumulative\n");
			fprintf(stderr, " -v add the 4th field of output\n");
            fprintf(stderr, " -i ignore empty file list; don't wait for stdin\n");
			exit(1);
		}
		argv++; argc--;
	}

	if (unix_style)
	{	fprintf(stdout, "    sloc     ncsl comments");
		if (verbose) fprintf(stdout, "       ;,");
		if (argc > 1) fprintf(stdout, "   file");
		fprintf(stdout, "\n");
	}

	if (argc == 1)
	{	if (!ignore_empty_files) {
            process_file(fd, "<stdin>");
            if (showlines)
                fprintf(stdout, "\n");
            if (unix_style)
            {	fprintf(stdout, "%8d %8d %8d",
                    raw, nc_linecount, n_comments);
                if (verbose)
                    fprintf(stdout, " %8d", semi_cnt);
                fprintf(stdout, "\n");
            }
            sum_rw = raw;
            sum_nc = nc_linecount;
            sum_cm = n_comments;
            sum_sm = semi_cnt;
	    }
	} else
		while (argc > 1)
		{	fd = fopen(argv[1], "r");
			if (fd == NULL)
			{	fprintf(stdout, "ncsl: cannot open %s\n", argv[1]);
				argv++; argc--;
				continue;
			}

			nrfiles++;
			relative = 0;
			state = PLAIN;
			process_file(fd, argv[1]);
			fclose(fd);

			if (showlines)
				fprintf(stdout, "\n");
			if (unix_style)
			{	fprintf(stdout, "%8d %8d %8d",
					raw, nc_linecount, n_comments);
				if (verbose)
					fprintf(stdout, " %8d ", semi_cnt);
				fprintf(stdout, "  %s\n", argv[1]);
			}
			sum_rw += raw; raw = 0;
			sum_nc += nc_linecount; nc_linecount = 0;
			sum_cm += n_comments; n_comments = 0;
			sum_sm += semi_cnt; semi_cnt = 0;
			argv++; argc--;
		}


	if (unix_style)
	{	//if (nrfiles > 1)
		{	fprintf(stdout, "%8d %8d %8d",
				sum_rw, sum_nc, sum_cm);
			if (verbose)
				fprintf(stdout, " %8d", sum_sm);
			fprintf(stdout, "  total \t[ncsl:sloc %6d%%]\n", (100*sum_nc)/(sum_rw+1));
		}
	} else
	{	fprintf(stdout,"SLOC: %6d\t(raw lines of source code)\n", sum_rw);
		fprintf(stdout,"NCSL: %6d\t(non-comment, non-blank lines of code)\n", sum_nc);
		fprintf(stdout,"CMNT: %6d\t(nr of comments)\n", sum_cm);
		if (verbose) {
		fprintf(stdout,"SSEPS:%6d\t(Statement separators: semicolons and commas)\n", sum_sm);
		fprintf(stdout,"Ratio:%6d%%\t(NCSL : SLOC)\n", (100*sum_nc)/(sum_rw+1));
	/*	fprintf(stdout,"TSEMI:%6d\t(Semicolons terminating a line)\n\n", t_semi_cnt); */
		}
	}
	exit(0);
}
