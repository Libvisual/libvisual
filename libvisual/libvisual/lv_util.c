#include "lv_util.h"
#include "lv_common.h"
#include <string.h>

char *visual_strdup (const char *s)
{
  size_t length = strlen (s);

  char *s2 = visual_mem_malloc ( + 1);

  return (char *) memcpy (s2, s, length);
}
