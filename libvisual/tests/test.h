#ifndef LV_TESTS_TEST_HPP
#define LV_TESTS_TEST_HPP

#include <stdio.h>
#include <stdlib.h>

#define LV_TEST_ASSERT(cond)                           \
    if (!(cond)) {                                     \
        fprintf (stderr, "Test failed: %s\n", #cond);  \
        exit (EXIT_FAILURE);                           \
    }

#endif // LV_TESTS_TEST_HPP
