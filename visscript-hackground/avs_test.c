#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "avs.h"

int main(int argc, char **argv)
{
	char buf[32768];
	int length;
	AvsRunnableContext *ctx;
	AvsNumber x, y;
	AvsRunnable *obj;
	int fd = 0;

//	rusty_mtrace("/tmp/mtrace.log");
	visual_init(&argc, &argv);
	visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);

	if (argc > 1)
		fd = open(argv[1], 0);
	
	length = read(fd, buf, sizeof(buf)-1);
	buf[length] = 0;
	
	ctx = avs_runnable_context_new();
	
	obj = avs_runnable_new(ctx);
	avs_runnable_variable_bind(obj, "x", &x);
	avs_runnable_variable_bind(obj, "y", &y);
	avs_runnable_compile(obj, buf, length);

	x = 0.0;
	y = 10.0;
	fprintf(stderr, "VM: var_x (%p) = %f var_y (%p) = %f\n", &x, x, &y, y);
	avs_runnable_execute(obj);
	fprintf(stderr, "VM: var_x (%p) = %f var_y (%p) = %f\n", &x, x, &y, y);

	
	visual_object_unref(VISUAL_OBJECT(obj));
	visual_object_unref(VISUAL_OBJECT(ctx));
	return 0;
}

