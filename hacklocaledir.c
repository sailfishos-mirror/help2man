/*
 * Nasty preload hack to allow message catalogs to be read from the "po"
 * subdir of the build tree.
 *
 * export LD_PRELOAD="hacklocaledir.so preloadable_libintl.so"
 * export TEXTDOMAIN=program
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#define PRELOAD "hacklocaledir.so"

static void *xmalloc(size_t len)
{
    void *r = malloc(len);
    if (!r)
    {
	fprintf(stderr, PRELOAD ": can't malloc %d bytes (%s)\n", len,
		strerror(errno));
	exit(1);
    }

    return r;
}

int __open(char const *path, int flags, mode_t mode)
{
    static int (*open)(char const *, int, mode_t) = 0;
    static char const *textdomain = 0;
    static char const *localedir = 0;
    static size_t localedirlen;
    static char *match = 0;
    static size_t matchlen;
    char *newpath = 0;
    int r;

    if (!open && !(open = dlsym(RTLD_NEXT, "open")))
    {
	fprintf(stderr, PRELOAD ": can't find open(): %s\n", dlerror());
	return -1;
    }

    if (textdomain || (textdomain = getenv("TEXTDOMAIN")))
    {
	size_t pathlen = strlen(path);
	char const *check;

	if (!localedir)
	{
	    if (!(localedir = getenv("LOCALEDIR")))
		localedir = "po";

	    localedirlen = strlen(localedir);
	}

	if (!match)
	{
	    matchlen = sizeof("/LC_MESSAGES/")-1 + strlen(textdomain)
	    	+ sizeof(".mo")-1;

	    match = xmalloc(matchlen + 1);
	    strcpy(match, "/LC_MESSAGES/");
	    strcat(match, textdomain);
	    strcat(match, ".mo");
	}

	if (*path == '/' && pathlen > matchlen &&
	    !strcmp(check = (path + pathlen - matchlen), match))
	{
	    char const *p = path;
	    char const *locale = 0;
	    do {
		locale = p + 1;
		p = strchr(locale, '/');
	    } while (p && p < check);

	    if (locale)
	    {
		size_t len = strcspn(locale, "/");
		newpath = xmalloc(localedirlen + 1 + len + sizeof(".gmo"));
		strcpy(newpath, localedir);
		strcat(newpath, "/");
		strncat(newpath, locale, len);
		strcat(newpath, ".gmo");

		if (access(newpath, R_OK))
		{
		    free(newpath);
		    newpath = 0;
		}
	    }
	}
    }

    r = (*open)(newpath ? newpath : path, flags, mode);
    if (newpath)
    {
	fprintf(stderr, "note: mapped %s to %s\n", path, newpath);
	free(newpath);
    }

    return r;
}

int open(char const *path, int flags, mode_t mode)
    __attribute__((weak, alias("__open")));
