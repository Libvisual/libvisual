#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "avs.h"

int verbose_level = 0;

void message(const int level, const char *format, ...)
{
    va_list ap;
    char buffer[256];

    if (level > verbose_level)
    return;

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    fprintf(level ? stdout : stderr, "%s\n", buffer);
}

