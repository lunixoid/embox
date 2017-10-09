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

#define IOMUXC_GPR3	0x20E000C

#define CCM_ANALOG_MISC1 0x20C8160

extern int clk_enable(char *clk_name);

static int ldb_init(void) {
	/* Enable LVDS1/2 output buffers */
	REG32_CLEAR(CCM_ANALOG_MISC1, (1 << 12) | (1 << 13));
	REG32_ORIN(CCM_ANALOG_MISC1, (1 << 10) | (1 << 11));

	clk_enable("ldb_di0");
	//clk_enable("ldb_di1");

	REG32_STORE(LDB_CTRL, LDB_CH0_DI0 | LDB_CH1_DI0);

	/****************************
	 * Configure IOMUX for LVDS:
	 *      IPU1 DI0 -> LVDS1
	 *      IPU1 DI0 -> LVDS2
	 ****************************/
	REG32_CLEAR(IOMUXC_GPR3, 3 << 8);
	REG32_CLEAR(IOMUXC_GPR3, 3 << 6);
	REG32_ORIN(IOMUXC_GPR3, 0 << 8);
	REG32_ORIN(IOMUXC_GPR3, 0 << 6);

	return 0;
}

static struct periph_memory_desc ldb_mem = {
	.start = LDB_CTRL,
	.len   = 0x4,
};

PERIPH_MEMORY_DEFINE(ldb_mem);

static struct periph_memory_desc ldb_iomux_mem = {
	.start = 0x20E0000,
	.len   = 0x1000,
};

PERIPH_MEMORY_DEFINE(ldb_iomux_mem);

static struct periph_memory_desc ccm_analog_mem = {
	.start = 0x20C8000,
	.len   = 0x200,
};

PERIPH_MEMORY_DEFINE(ccm_analog_mem);
