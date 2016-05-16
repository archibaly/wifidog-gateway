#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include "debug.h"
#include "openmax.h"

#ifdef OPEN_MAX
static int openmax = OPEN_MAX;
#else
static int openmax = 0;
#endif

/*
 * If OPEN_MAX is indeterminate, we're not
 * guaranteed that this is adequate.
 */
#define	OPEN_MAX_GUESS	256

int open_max(void)
{
	if (openmax == 0) {		/* first time through */
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;	/* it's indeterminate */
			else
				debug(LOG_ERR, "sysconf error for _SC_OPEN_MAX");
		}
	}

	return(openmax);
}
