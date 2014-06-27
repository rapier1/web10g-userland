#if !defined(UTIL_SCRIPTS_SCRIPTS_H)
#define UTIL_SCRIPTS_SCRIPTS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <estats/estats.h>

#define Chk(x)					\
	do {					\
		err = (x);			\
		if (err != NULL) {		\
			goto Cleanup;		\
		}				\
	} while (0)

/* we need a different macro that does not go to Cleanup, but just to the end of the loop */
#define Chk2(x)								\
        do {								\
                err = (x);                                              \
                if (err != NULL) {                                      \
                        dbgprintf("   ... saw error \"%s\" (error code %d) at %s:%d in function %s (\"%s\")\n", \
                                  estats_error_get_message(err),        \
                                  estats_error_get_number(err),         \
                                  __FILE__,                             \
                                  __LINE__,                             \
                                  __FUNCTION__,                         \
                                  estats_error_get_extra(err));         \
			estats_error_free(&err);			\
                        goto Continue;                                  \
                }                                                       \
        } while (0)

/* we need a different macro that does not go to Cleanup, but just to the end of the loop */
#define Chk2Ign(x)							\
        do {								\
                err = (x);                                              \
                if (err != NULL) {                                      \
			estats_error_free(&err);			\
                        goto Continue;                                  \
                }                                                       \
        } while (0)


#define ChkIgn(x)					\
	do {						\
		err = (x);				\
		if (err != NULL) {			\
			estats_error_free(&err);	\
			goto Cleanup;			\
		}					\
	} while (0)

#define SWAP(x, y)				\
	do {					\
		typeof(x) tmp;			\
		tmp = x;			\
		x = y;				\
		y = tmp;			\
	} while (0)

#define PRINT_AND_FREE(err)				\
	do {						\
		estats_error_print(stderr, err);	\
		estats_error_free(&err);		\
	} while (0)

#endif /* !defined(UTIL_SCRIPTS_SCRIPTS_H) */
