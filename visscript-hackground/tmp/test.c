#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libvisual/libvisual.h>

#define AVSTEST(obj)	(VISUAL_CHECK_CAST((obj), 0, AVSTest))

struct _AVSTest {
	VisObject	object;
	char		*turk;
};

typedef struct _AVSTest AVSTest;

static int avs_test_dtor (VisObject *object)
{
	AVSTest *t = AVSTEST(object);
	printf("FREE FUCKER!\n");
	return VISUAL_OK;
}

int main(void)
{
	AVSTest *t;

	t = visual_mem_new0(AVSTest, 1);
	visual_object_initialize(VISUAL_OBJECT(t), TRUE, avs_test_dtor);
	visual_object_unref(VISUAL_OBJECT(t));
	return 0;
}
