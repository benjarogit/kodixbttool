/*
 * getopt_win.h – getopt/getopt_long for Windows (used when building on WIN32).
 * Based on BSD getopt from libevent WIN32-Code (University of California).
 * This file is part of kodixbttool; include only on Windows builds.
 */
#ifndef GETOPT_WIN_H
#define GETOPT_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

extern int opterr;
extern int optind;
extern int optopt;
extern int optreset;
extern char *optarg;

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

#define no_argument         0
#define required_argument  1
#define optional_argument  2

int getopt(int argc, char *const *argv, const char *optstring);
int getopt_long(int argc, char *const *argv, const char *optstring,
                const struct option *longopts, int *longindex);

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_WIN_H */
