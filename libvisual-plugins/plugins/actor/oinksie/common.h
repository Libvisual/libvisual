#ifndef _OINK_COMMON_H
#define _OINK_COMMON_H

#include <stdlib.h>
#include <unistd.h>

/* FIXME check this with configure */
#define inline __inline__

#define PI	3.14159265358979323846

#define MAX(a, b) (a > b ? a : b)
#define MIN(a ,b) (a > b ? b : a)

#define BYTETRUNC(a) ((a) > 255 ? 255 : (a))

#endif /* _OINK_COMMON_H */
