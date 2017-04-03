/**
 * @file
 *
 * @date 31.03.2017
 * @author Anton Bondarev
 */
#include <stdint.h>
#include <string.h>

#include <hal/reg.h>

#include <util/array.h>
#include <util/log.h>

#include <framework/mod/options.h>

#define CCM_BASE           OPTION_GET(NUMBER, base_addr)

#define MXC_CCM_CCR         (CCM_BASE + 0x00)
#define MXC_CCM_CCDR        (CCM_BASE + 0x04)
#define MXC_CCM_CSR         (CCM_BASE + 0x08)
#define MXC_CCM_CCSR        (CCM_BASE + 0x0c)
#define MXC_CCM_CACRR       (CCM_BASE + 0x10)
#define MXC_CCM_CBCDR       (CCM_BASE + 0x14)
#define MXC_CCM_CBCMR       (CCM_BASE + 0x18)
#define MXC_CCM_CSCMR1      (CCM_BASE + 0x1c)
#define MXC_CCM_CSCMR2      (CCM_BASE + 0x20)
#define MXC_CCM_CSCDR1      (CCM_BASE + 0x24)
#define MXC_CCM_CS1CDR      (CCM_BASE + 0x28)
#define MXC_CCM_CS2CDR      (CCM_BASE + 0x2c)
#define MXC_CCM_CDCDR       (CCM_BASE + 0x30)
#define MXC_CCM_CHSCDR      (CCM_BASE + 0x34)
#define MXC_CCM_CSCDR2      (CCM_BASE + 0x38)
#define MXC_CCM_CSCDR3      (CCM_BASE + 0x3c)

//#define MXC_CCM_CSCDR4     (CCM_BASE + 0x40)
#define MXC_CCM_CWDR        (CCM_BASE + 0x44)
#define MXC_CCM_CDHIPR      (CCM_BASE + 0x48)
//#define MXC_CCM_CDCR       (CCM_BASE + 0x4c)
//#define MXC_CCM_CTOR       (CCM_BASE + 0x50)
//#define MXC_CCM_CLPCR      (CCM_BASE + 0x54)
#define MXC_CCM_CISR        (CCM_BASE + 0x58)
#define MXC_CCM_CIMR        (CCM_BASE + 0x5c)
#define MXC_CCM_CCOSR       (CCM_BASE + 0x60)
#define MXC_CCM_CGPR        (CCM_BASE + 0x64)
#define MXC_CCM_CCGR0       (CCM_BASE + 0x68)
#define MXC_CCM_CCGR1       (CCM_BASE + 0x6c)
#define MXC_CCM_CCGR2       (CCM_BASE + 0x70)
#define MXC_CCM_CCGR3       (CCM_BASE + 0x74)
#define MXC_CCM_CCGR4       (CCM_BASE + 0x78)
#define MXC_CCM_CCGR5       (CCM_BASE + 0x7c)
#define MXC_CCM_CCGR6       (CCM_BASE + 0x80)
//#define MXC_CCM_CCGR7      (CCM_BASE + 0x84)
#define MXC_CCM_CMEOR       (CCM_BASE + 0x84)

struct clk {
	uint32_t reg_offset;
	int bit_num;
	const char *clk_name;
};

static const struct clk clks_repo[] = {
	{ MXC_CCM_CCGR2, 0, "iahb" },
	{ MXC_CCM_CCGR2, 2, "isfr" },
};

int clk_enable(char *clk_name) {
	int i;

	for (i = 0; i < ARRAY_SIZE(clks_repo); i ++) {
		if (0 == strcmp(clks_repo[i].clk_name, clk_name)) {
			uint32_t reg;

			reg = REG32_LOAD(clks_repo[i].reg_offset);
			reg |= 1 << clks_repo[i].bit_num;
			REG32_STORE(clks_repo[i].reg_offset, reg);

			return i;
		}
	}

	return -1; 
}

