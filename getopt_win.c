/*
 * getopt_win.c – getopt/getopt_long for Windows.
 * Based on getopt_long.c from libevent WIN32-Code (BSD-style).
 * Copyright (c) 1987, 1993, 1994, 1996 The Regents of the University of California.
 * This file is part of kodixbttool; compile only on Windows builds.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt_win.h"

int opterr = 1;
int optind = 1;
int optopt;
int optreset = 0;
char *optarg = NULL;

static const char *progname(const char *nargv0)
{
	const char *tmp;
	if (!nargv0) return "";
	tmp = strrchr(nargv0, '/');
	if (tmp) tmp++;
	else tmp = nargv0;
#ifdef _WIN32
	{
		const char *b = strrchr(nargv0, '\\');
		if (b && b > tmp) tmp = b + 1;
	}
#endif
	return tmp;
}

#define BADCH  ((int)'?')
#define BADARG ((int)':')
#define EMSG   ""

static int getopt_internal(int nargc, char *const *nargv, const char *ostr)
{
	static char *place = EMSG;

	if (optreset || !*place) {
		optreset = 0;
		if (optind >= nargc || *(place = (char *)nargv[optind]) != '-') {
			place = EMSG;
			return -1;
		}
		if (place[1] && *++place == '-') {
			place = EMSG;
			return -2;
		}
	}
	optopt = (int)*place++;
	if (optopt == (int)':' || !strchr(ostr, optopt)) {
		if (optopt == (int)'-')
			return -1;
		if (!*place)
			++optind;
		if (opterr && *ostr != ':')
			fprintf(stderr, "%s: illegal option -- %c\n", progname(nargv[0]), optopt);
		return BADCH;
	}
	if (*strchr(ostr, optopt) != ':') {
		optarg = NULL;
		if (!*place)
			++optind;
	} else {
		if (*place)
			optarg = place;
		else if (nargc <= ++optind) {
			place = EMSG;
			if (opterr && *ostr != ':')
				fprintf(stderr, "%s: option requires an argument -- %c\n",
					progname(nargv[0]), optopt);
			return BADARG;
		} else
			optarg = (char *)nargv[optind];
		place = EMSG;
		++optind;
	}
	return optopt;
}

int getopt(int argc, char *const *argv, const char *optstring)
{
	int ret = getopt_internal(argc, argv, optstring);
	if (ret == -2) {
		++optind;
		ret = -1;
	}
	return ret;
}

int getopt_long(int argc, char *const *argv, const char *optstring,
                const struct option *longopts, int *longindex)
{
	int ret = getopt_internal(argc, (char *const *)argv, optstring);
	if (ret != -2)
		return ret;

	char *current_argv = (char *)argv[optind++] + 2;
	char *has_equal;
	size_t current_argv_len;
	int i, match = -1;

	if (*current_argv == '\0')
		return -1;
	has_equal = strchr(current_argv, '=');
	if (has_equal) {
		current_argv_len = (size_t)(has_equal - current_argv);
		has_equal++;
	} else
		current_argv_len = strlen(current_argv);

	for (i = 0; longopts[i].name; i++) {
		if (strncmp(current_argv, longopts[i].name, current_argv_len) != 0)
			continue;
		if (strlen(longopts[i].name) == current_argv_len) {
			match = i;
			break;
		}
		if (match < 0)
			match = i;
	}
	if (match >= 0) {
		if (longopts[match].has_arg == required_argument ||
		    longopts[match].has_arg == optional_argument) {
			if (has_equal)
				optarg = has_equal;
			else
				optarg = (char *)argv[optind++];
		}
		if (longopts[match].has_arg == required_argument && !optarg) {
			if (opterr && *optstring != ':')
				fprintf(stderr, "%s: option requires an argument -- %s\n",
					progname(argv[0]), current_argv);
			return BADARG;
		}
		if (longopts[match].flag)
			*longopts[match].flag = longopts[match].val;
		else
			ret = longopts[match].val;
		if (longindex)
			*longindex = match;
	} else {
		if (opterr && *optstring != ':')
			fprintf(stderr, "%s: illegal option -- %s\n", progname(argv[0]), current_argv);
		return BADCH;
	}
	return ret;
}
