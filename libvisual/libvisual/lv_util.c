#include "lv_util.h"
#include "lv_common.h"
#include <string.h>

char *visual_strdup (const char *str)
{
  size_t size = strlen (str) + 1;
  char *new_str = visual_mem_malloc (size);

  return (char *) memcpy (new_str, str, size);
}
