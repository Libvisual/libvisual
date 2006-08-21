#ifndef _UNIT_TEST_H
#define _UNIT_TEST_H 1

typedef void (*unit_test_call)(int *, char **);
typedef void (*unit_test_setup)(void);
typedef void (*unit_test_teardown)(void);

typedef struct unit_test 
{
	char		*name;
	unit_test_call	call;
} unit_test;

typedef struct unit_test_descriptor
{
	char			*unitname;
	unit_test_setup		setup;
	unit_test_teardown	teardown;
	unit_test		tests[];
} unit_test_descriptor;


#define __expected_error(buf, fmt, cmp, exp, got) \
	asprintf(buf, "Expected value '" fmt "' to be " cmp " '" fmt "'! (%s:%d)", got, exp, __FILE__, __LINE__)

#define __expect(type, comparator, cmp_lit, cmp_args, fmt, expected_value, value) \
	{ \
		type __evalue = (type) (expected_value);\
		type __gvalue = (type) (value); \
		if (!comparator(__evalue, __gvalue, cmp_args)) { \
			*__result = -1; \
			__expected_error(__reason, fmt, cmp_lit, __evalue, __gvalue); \
			return;\
		}\
	}

#define __cmp_string(exp, got, args) !strcmp(exp, got)
#define __cmp_mem(exp, got, args) !memcmp(exp, got, args)
#define __cmp_integer(exp, got, args) ((exp) == (got))
#define __cmp_integer_eq(exp, got, args) ((exp) == (got))
#define __cmp_integer_ne(exp, got, args) ((exp) != (got))
#define __cmp_integer_lt(exp, got, args) ((exp) > (got))
#define __cmp_integer_gt(exp, got, args) ((exp) < (got))
#define __cmp_integer_le(exp, got, args) ((exp) >= (got))
#define __cmp_integer_ge(exp, got, args) ((exp) <= (got))
#define __cmp_expression(exp, got, args) !(got)

#define expect_string(expected_value, value) __expect(char *, __cmp_string, "equal to",, "%s", expected_value, value)
#define expect_mem(expected_value, value, len) __expect(void *, __cmp_mem, "equal to", len, "%p", expected_value, value)
#define expect_integer(expected_value, value) __expect(int, __cmp_integer, "==",, "%d", expected_value, value)
#define expect_integer_eq(expected_value, value) __expect(int, __cmp_integer_eq, "==",, "%d", expected_value, value)
#define expect_integer_ne(expected_value, value) __expect(int, __cmp_integer_ne, "!=",, "%d", expected_value, value)
#define expect_integer_lt(expected_value, value) __expect(int, __cmp_integer_lt, "<",, "%d", expected_value, value)
#define expect_integer_gt(expected_value, value) __expect(int, __cmp_integer_gt, ">",, "%d", expected_value, value)
#define expect_integer_le(expected_value, value) __expect(int, __cmp_integer_le, "<=",, "%d", expected_value, value)
#define expect_integer_ge(expected_value, value) __expect(int, __cmp_integer_ge, ">=",, "%d", expected_value, value)

#define expect_eval(expression) __expect(int, __cmp_expression, "%d", 0, expression)

#define unit_test_debug(fmt, args...) \
	fprintf(stderr, ">> " fmt, ##args)

#define unit_test_print(fmt, args...) \
	fprintf(stderr, fmt, ##args)

static inline int __run_unit_test(unit_test_descriptor *desc)
{
	unit_test *test;
	char *reason;
	int i, result, tests = 0, successful = 0;

	unit_test_debug("Running unit test: %s\n", desc->unitname);

	for (i=0; desc->tests[i].name != NULL; i++) {
		tests++;

		if (desc->setup)
			desc->setup();

		test = &desc->tests[i];
		unit_test_print("Testing %s ... ", test->name);
		result = 0;
		reason = NULL;
		test->call(&result, &reason);
		
		if (desc->teardown)
			desc->teardown();

		if (result) {
			unit_test_print("FAILED!\n");
			if (reason)
				unit_test_print("... Reason: %s\n", reason);
		} else {
			successful++;
			unit_test_print("PASSED!\n");
		}
	}

	unit_test_debug("\n");
	unit_test_debug("Unit test finished: \n");
	unit_test_debug("\tTotal number of tests : %d\n", tests);
	unit_test_debug("\tNumber of tests PASSED: %d\n", successful);
	unit_test_debug("\tNumber of tests FAILED: %d\n", tests - successful);
	unit_test_debug("\n");
	unit_test_debug("\tPercentage            : %.2f%\n", (double)successful/(double)tests*100.0);
	unit_test_debug("\n");
	unit_test_debug("Unit test verdict: %s\n", (tests == successful ? "SUCCESS!" : "FAILURE!"));
	return (tests != successful);
}

#define run_unit_test(x) \
	int main(int argc, char **argv) { return __run_unit_test(x); }

#define UNIT_TEST_SETUP(fn) \
	static void fn (void)

#define UNIT_TEST_TEARDOWN(fn) \
	static void fn (void)

#define UNIT_TEST(fn) \
	static void fn (int *__result, char **__reason)

#define UNIT_TEST_INCLUDE(name, fn) \
	{ name, fn, } 

#define UNIT_TEST_RUN(xunitname, xsetup, xteardown, xtests...) \
	static unit_test_descriptor descriptor = { \
		.unitname = xunitname, \
		.setup = xsetup, \
		.teardown = xteardown, \
		{ \
			xtests, \
			{ NULL, NULL, }, \
		}, \
	}; \
	run_unit_test(&descriptor)


#endif /* !_UNIT_TEST_H */
