//
//  dirhelper_mkdir_p.c
//  libcoreservices
//
//  Created by William Kent on 11/7/18.
//  Copyright © 2018 PureDarwin Project. All rights reserved.
//

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "dirhelper_priv.h"
#include "CrashReporterClient.h"

void __attribute__((__format__(__printf__,1,2)))
_setcrashlogmessage(const char *fmt, ...)
{
	char *mess = NULL;
	int res;
	va_list ap;

	va_start(ap, fmt);
	res = vasprintf(&mess, fmt, ap);
	va_end(ap);
	if (res < 0)
		mess = (char *)fmt; /* the format string is better than nothing */
	CRSetCrashLogMessage(mess);
}

char *
__user_local_mkdir_p(char *path)
{
	char *next;
	int res;

	next = path + strlen(VAR_FOLDERS_PATH);
	while ((next = strchr(next, '/')) != NULL) {
		*next = 0; // temporarily truncate
		res = mkdir(path, 0755);
		if (res != 0 && errno != EEXIST) {
			setcrashlogmessage("mkdir: path=%s mode=0755: %s", path, strerror(errno));
			return NULL;
		}
		*next++ = '/'; // restore the slash and increment
	}
	return path;
}

