/**
 * @file ldb.c
 * @brief LVDS Display Bridge
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.10.2017
 */

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <framework/mod/options.h>

static int ldb_init(void);
EMBOX_UNIT_INIT(ldb_init);

#define LDB_CTRL	OPTION_GET(NUMBER,base_addr)

#define LDB_CH0_OFF     0x0
#define LDB_CH0_DI0     0x1
#define LDB_CH0_DI1     0x3

#define LDB_CH1_OFF     0x0
#define LDB_CH1_DI0     0x4
#define LDB_CH1_DI1     0xC

static int ldb_init(void) {
	/* Enable clock? CCM_CCGR3 */
	REG32_STORE(LDB_CTRL, LDB_CH0_DI0 | LDB_CH1_DI0);

	return 0;
}

static struct periph_memory_desc ldb_mem = {
	.start = LDB_CTRL,
	.len   = 0x4,
};

PERIPH_MEMORY_DEFINE(ldb_mem);
