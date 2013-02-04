#include "lv_util.h"
#include "lv_common.h"
#include <string.h>

char *visual_strdup (const char *str)
{
    size_t size = strlen (str) + 1;
    char *new_str = visual_mem_malloc (size);

    return (char *) memcpy (new_str, str, size);
}

const char *visual_truncate_path (const char* filename, unsigned int parts)
{
    /* Start looking for '/' from the end of the filename */
    const char *s = filename + strlen (filename);

    unsigned int i;

    for (i = 0; i < parts; i++) {
        /* Scan backwards until we hit '/' or the beginning of the string */
        while (s != filename && *s != '/')
            s--;

        /* If we hit the beginning, we just return the filename, unmodified */
        if (s == filename)
            return filename;

        /* Skip this instance of / */
        s--;
    }

    /* We found the final /. Return the substring that begins right after it */
    return s + 2;
}
