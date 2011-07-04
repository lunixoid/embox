/**
 * @file
 * @brief TODO This file is derived from Embox test template.
 *
 * @date
 * @author TODO Your name here
 */

#include <embox/test.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	printf("wait 2sec ... ");
	sleep(2);
	printf("ok ");

	return result;
}
