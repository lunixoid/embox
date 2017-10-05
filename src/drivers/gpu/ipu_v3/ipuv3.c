/**
 * @file
 *
 * @data Sep 15, 2017
 * @author Anton Bondarev
 */
#include <util/log.h>
#include <hal/reg.h>

#include "ipu_regs.h"

int ipu_setup_params(int ch) {
	return 0;
}

int ipu_enable_channel(int ch) {
	return 0;
}

int ipu_disable_channel(int ch) {
	/* + uninit */
	return 0;
}

int ipu_init_sync_panel(void) {
	return 0;
}

int ipu_probe(void)
{
	int i;
	/* Enable IPU clock */

	/* Clear internal memories of IPU */
	REG32_STORE(IPU_MEM_RST, 0x807FFFFF);
	while (REG32_LOAD(IPU_MEM_RST) & 0x80000000);

	/* Init display controller mappings */

	/* Disable all interrupts */
	for (i = 1; i < 16; i++) {
		REG32_STORE(IPU_INT_CTRL(i), 0);
	}

	/* Init DMFC */

	/* Setup priority */

	/* Disable IPU clock */
	return 0;
}
