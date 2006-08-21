#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libvisual/libvisual.h>

#include "unit_test.h"


UNIT_TEST(test_version)
{
	expect_integer_ge(4000, visual_get_api_version());
}

UNIT_TEST(test_config_version)
{
	expect_string(VISUAL_CONFIG_VERSION, "LV CONFIG FILE  3");
}

UNIT_TEST(test_plugin_version)
{
	expect_integer_ge(3000, VISUAL_PLUGIN_API_VERSION);
}

UNIT_TEST_RUN(
	"Version test",
	NULL,
	NULL,
	UNIT_TEST_INCLUDE("version of libvisual API >= 4.0", test_version),
	UNIT_TEST_INCLUDE("version of plugin API >= 3.0", test_plugin_version), 
	UNIT_TEST_INCLUDE("version of config registry == 3.0", test_config_version)
);
