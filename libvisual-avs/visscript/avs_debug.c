#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <libvisual/libvisual.h>

#include "avs.h"

void message(const VisLogVerboseness level, const char *format, ...)
{
    va_list ap;
    char buffer[256];
    char tag[32];
    char *info = "VisScript INFO: ";
    char *debug = "VisScript DEBUG: ";
    char *error = "VisScript ERROR: ";

return;
    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if(visual_log_get_verboseness() < level)
        return;

    if(level == VISUAL_LOG_VERBOSENESS_LOW)
        snprintf(tag, 31, error);
    else if (level == VISUAL_LOG_VERBOSENESS_MEDIUM)
        snprintf(tag, 31, debug);
    else if (level == VISUAL_LOG_VERBOSENESS_HIGH)
        snprintf(tag, 31, info);
    else 
        return;

    fprintf(level == VISUAL_LOG_VERBOSENESS_LOW ? stdout : stderr, "%s%s\n", tag, buffer);
}

