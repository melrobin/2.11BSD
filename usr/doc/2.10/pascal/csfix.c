#include <stdio.h>
/*
 * csfix - fix constant spacing for error message flags in troff
 *
 * Bill Joy UCB September 11, 1977
 *
 * This would be better written in snobol!
 *
 * Normally fixes error flags in a pi listing
 * Optional - causes fixing of '---' and initial blank widthin a pxp profile.
 */

char	flag, dflag;

main(argc, argv)
	int argc;
	char *argv[];
{

	argc--, argv++;
	if (argc > 0 && argv[0][0] == '-' && argv[0][1] == 'd')
		dflag++, argc--, argv++;
	if (argc > 0 && argv[0][0] == '-')
		flag++, argc--, argv++;
	if (argc != 0) {
		write(2, "Usage: csfix\n", 13);
		exit(1);
	}
	while (getline()) {
		if (errline()) {
			flag ? fixpxp() : reformat();
			continue;
		}
		if (flag) {
			fixdigits();
			continue;
		}
		if (spwarn())
			continue;
		if (nontriv())
			save();
		if (dflag)
			fixdigits();
		else
			putline();
	}
	exit(0);
}

char	line[160], flagee[160];

getline()
{
	register char *cp, c;

	for (cp = line, c = getchar(); c != '\n' && c != EOF; c = getchar())
		*cp++ = c;
	if (c == EOF)
		return (0);
	*cp = 0;
	return (1);
}

errline()
{
	register int i;
	register char *cp;

	for (cp = line; cp[0] && cp[1] && cp[2]; cp++)
		if (cp[0] == '-' && cp[1] == '-' && cp[2] == '-')
			return (1);
	return (0);
}

reformat()
{
	register char *cp, c, *tail;

	printf("%2.2s", line);
	if (line[0] != 'w')
		printf("\\l'\\w`w `u-\\w`%2.2s`u '", line);
	for (cp = line; *cp != 0 && *cp != '^'; cp++)
		continue;
	tail = cp + 1;
	if (cp[-1] == '\b' && cp[-2] == '|')
		cp =- 2;
	c = flagee[cp - line];
	flagee[cp - line] = 0;
	printf("\\l'\\w`%s`u-\\w`w `u\\&\\(rh'", flagee);
	flagee[cp - line] = c;
	printf("\\l'(\\w`%c`u-\\w`^`u)/2 '", c);
	printf("\\(ua");
	printf("\\l'(\\w`%c`u-\\w`^`u)/2 '", c);
	printf("\\l'\\w`---`u\\&\\(rh'%s\n", tail+3);
}

nontriv()
{

	switch (line[0]) {
		case 'E':
		case 'e':
		case 'w':
		case 's':
		case 0:
			return (0);
	}
	return (1);
}

save()
{

	strcpy(flagee, line);
}

putline()
{

	printf("%s\n", flag ? digitty(0) : line);
}

spwarn()
{

	if (line[0] != ' ' || line[1] != ' ' || line[2] != 'w')
		return (0);
	printf("  \\l'(\\w`E`u-\\w`w`u)/2 'w\\l'(\\w`E`u-\\w`w`u)/2 '");
	printf(&line[3]);
	printf("\n");
	return (1);
}

fixpxp()
{
	register char *cp;

	for (cp = line; *cp != '-'; cp++)
		continue;
	*cp = 0;
	printf("%s\\l'\\w`\\0\\0\\0\\0`u-\\w`.`u\\&\\(rh'%s\n", digitty(1), cp + 3);
}

digitty(yup)
	char yup;
{
	register char *cp, *dp, *lp;

	for (lp = line; *lp && *lp != '|'; lp++)
		continue;
	if (yup == 0 && !*lp)
		return (line);
	for (cp = line, dp = flagee; cp < lp; cp++)
		if (*cp == ' ')
			*dp++ = '\\', *dp++ = '0';
		else
			*dp++ = *cp;
	strcpy(dp, cp);
	return (flagee);
}

fixdigits()
{
	register char *cp, c;

	for (cp = line; *cp == ' ' || *cp >= '0' && *cp <= '9'; cp++)
		continue;
	c = *cp, *cp = 0;
	digitty(1);
	*cp = c;
	printf("%s%s\n", flagee, cp);
}
