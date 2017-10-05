/**
 * @file
 *
 * @data Sep 15, 2017
 * @author Anton Bondarev
 */
#include <hal/reg.h>
#include <drivers/video/fb.h>
#include <util/log.h>

#include "ipu_regs.h"

/* Default config      Parameters            Possible values */
static const int ipu_word_size_bits = 24; /* 8, 16, 24 or 32 */
static const int ipu_di = 0;              /* 0..1 */
static const int ipu_display = 2;         /* 0..3 */

int ipu_setup_params() {
	log_debug("%s is NIY", __func__);
	return 0;
}

int ipu_dc_init(int ch) {
	uint32_t conf = 0x0;

	switch (ipu_word_size_bits) {
	case 8:
		break;
	case 16:
		conf |= 0x1;
		break;
	case 24:
		conf |= 0x2;
		break;
	case 32:
		conf |= 0x3;
		break;
	default:
		log_error("Wrong bits per word=%d, assume 8 bits",
				ipu_word_size_bits);
		break;
	}

	conf |= ipu_di << 2;
	conf |= ipu_display << 3;
	conf |= 8 << 5; /* Normal mode without anti-tearing */

	REG32_STORE(conf, IPU_DC_WR_CH_CONF(ch));
	REG32_STORE(0, IPU_DC_WR_CH_ADDR(ch));
	REG32_STORE(0x84, IPU_DC_GEN);

	return 0;
}

int ipu_init_channel(int ch) {
	uint32_t reg;

	ipu_dc_init(ch);

	reg = REG32_LOAD(IPU_CONF);
	reg |= 1 << 22;

	if (ipu_di == 0) {
		reg |= 1 << 6;
	} else {
		reg |= 1 << 7;
	}

	return 0;
}

int ipu_enable_channel(int ch) {
	log_debug("%s is NIY", __func__);
	return 0;
}

int ipu_disable_channel(int ch) {
	log_debug("%s is NIY", __func__);
	/* + uninit */
	return 0;
}

int ipu_init_sync_panel(int ch) {
	log_debug("%s is NIY", __func__);
	return 0;
}

int ipu_init_channel_buffer(struct fb_info *fbi) {
	log_debug("%s is NIY", __func__);
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
