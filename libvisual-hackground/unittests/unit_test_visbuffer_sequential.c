#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libvisual/libvisual.h>

#include "unit_test.h"


static VisBuffer *buf;

UNIT_TEST_SETUP(setup)
{
	buf = visual_buffer_new_allocate(4096, visual_buffer_destroyer_free);
}

UNIT_TEST_TEARDOWN(teardown)
{
	visual_object_unref(VISUAL_OBJECT(buf));
}

UNIT_TEST(test_init_offset)
{
	expect_integer(0, visual_buffer_get_offset_sequential(buf));
}

UNIT_TEST(test_set_offset)
{
	expect_integer(VISUAL_OK, visual_buffer_set_offset_sequential(buf, 42));
	expect_integer(42, visual_buffer_get_offset_sequential(buf));

	expect_integer(VISUAL_OK, visual_buffer_set_offset_sequential(buf, 0));
	expect_integer(0, visual_buffer_get_offset_sequential(buf));
}

UNIT_TEST(test_set_offset_oob)
{
	expect_integer_eq(VISUAL_OK, visual_buffer_set_offset_sequential(buf, visual_buffer_get_size(buf)));
	expect_integer_eq(VISUAL_OK, visual_buffer_set_offset_sequential(buf, 0));
	expect_integer_ne(VISUAL_OK, visual_buffer_set_offset_sequential(buf, -1));
	expect_integer_ne(VISUAL_OK, visual_buffer_set_offset_sequential(buf, visual_buffer_get_size(buf)+1));
}

UNIT_TEST(test_reset)
{
	expect_integer_eq(VISUAL_OK, visual_buffer_set_offset_sequential(buf, 42));
	expect_integer_eq(VISUAL_OK, visual_buffer_reset_offset_sequential(buf));
	expect_integer_eq(0, visual_buffer_get_offset_sequential(buf));
}

UNIT_TEST(test_get_data)
{
	expect_integer_eq(visual_buffer_get_data(buf), visual_buffer_get_data_offset_sequential(buf));
	visual_buffer_set_offset_sequential(buf, 42);
	expect_integer_eq(visual_buffer_get_data(buf)+42, visual_buffer_get_data_offset_sequential(buf));
}

UNIT_TEST(test_put_data)
{
	int i;
	VisBuffer *testpat = visual_buffer_new_allocate(8, visual_buffer_destroyer_free);

	for (i=0; i < visual_buffer_get_size(buf); i += visual_buffer_get_size(testpat)) {
		visual_buffer_fill(testpat, i % 42);
		expect_integer_eq(VISUAL_OK, visual_buffer_put_sequential(buf, testpat));
	}

	// compare
	for (i=0; i < visual_buffer_get_size(buf); i += visual_buffer_get_size(testpat)) {
		char *cpat, *spat;
		visual_buffer_fill(testpat, i % 42);
		expect_integer_ne(NULL, cpat = visual_buffer_get_data(testpat));
		expect_integer_ne(NULL, spat = visual_buffer_get_data_offset(buf, i));
		expect_mem(cpat, spat, visual_buffer_get_size(testpat));
	}
}

UNIT_TEST(test_put_data_atomic)
{
	VisBuffer *data = visual_buffer_new_allocate(visual_buffer_get_size(buf) / 2 + 2, visual_buffer_destroyer_free);
	expect_integer_eq(VISUAL_OK, visual_buffer_put_sequential_atomic(buf, data));
	expect_integer_ne(VISUAL_OK, visual_buffer_put_sequential_atomic(buf, data));
	visual_buffer_reset_offset_sequential(buf);
	expect_integer_eq(VISUAL_OK, visual_buffer_put_sequential(buf, data));
	expect_integer_eq(VISUAL_OK, visual_buffer_put_sequential(buf, data));
	expect_integer_eq(VISUAL_OK, visual_buffer_put_sequential(buf, data));
	expect_integer_eq(0, visual_buffer_get_available_sequential(buf));
}

UNIT_TEST(test_clone)
{
	VisBuffer *dest = visual_buffer_new();
	visual_buffer_set_offset_sequential(buf, 42);
	expect_integer_eq(VISUAL_OK, visual_buffer_clone_with_attributes(dest, buf));
	expect_integer_eq(visual_buffer_get_offset_sequential(buf), 
			  visual_buffer_get_offset_sequential(dest));
}

UNIT_TEST(test_clone_new)
{
	VisBuffer *dest;
	visual_buffer_set_offset_sequential(buf, 42);
	expect_integer_ne(NULL, dest = visual_buffer_clone_new_with_attributes(buf));
	expect_integer_eq(visual_buffer_get_offset_sequential(buf), 
			  visual_buffer_get_offset_sequential(dest));
}

UNIT_TEST(test_available_space)
{
	expect_integer_eq(visual_buffer_get_size(buf), visual_buffer_get_available_sequential(buf));
	expect_integer_eq(VISUAL_OK, visual_buffer_set_offset_sequential(buf, 10));
	expect_integer_eq(visual_buffer_get_size(buf)-10, visual_buffer_get_available_sequential(buf));
	expect_integer_eq(VISUAL_OK, visual_buffer_set_offset_sequential(buf, visual_buffer_get_size(buf)));
	expect_integer_eq(0, visual_buffer_get_available_sequential(buf));
}

UNIT_TEST_RUN(
	"VisBuffer Sequential Offset",
	setup,
	teardown,
	UNIT_TEST_INCLUDE("on init offset == 0", test_init_offset),
	UNIT_TEST_INCLUDE("setting sequential offset", test_set_offset), 
	UNIT_TEST_INCLUDE("setting sequential offset out of bounds", test_set_offset_oob),
	UNIT_TEST_INCLUDE("resetting the offset", test_reset),
	UNIT_TEST_INCLUDE("getting data from sequential offset", test_get_data),
	UNIT_TEST_INCLUDE("putting data at sequential offset", test_put_data),
	UNIT_TEST_INCLUDE("putting data at sequential offset atomically", test_put_data_atomic),
	UNIT_TEST_INCLUDE("cloning a buffer with attributes", test_clone),
	UNIT_TEST_INCLUDE("cloning a buffer into a new buffer with attributes", test_clone_new),
	UNIT_TEST_INCLUDE("available space", test_available_space)
);
