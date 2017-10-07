/**
 * @file
 *
 * @data Sep 15, 2017
 * @author Anton Bondarev
 */
#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/video/fb.h>
#include <kernel/irq.h>
#include <string.h>
#include <util/log.h>

#include "ipu_regs.h"

struct ipu_ch_param_word {
	uint32_t data[5];
	uint32_t res[3];
};

struct ipu_ch_param {
	struct ipu_ch_param_word word[2];
};

#define ipu_ch_param_addr(ch) (((struct ipu_ch_param *) IPU_CPMEM) + (ch))

#define _param_word(base, w) \
	(((struct ipu_ch_param *)(base))->word[(w)].data)

#define ipu_ch_param_set_field(base, w, bit, size, v) { \
	int i = (bit) / 32; \
	int off = (bit) % 32; \
	_param_word(base, w)[i] |= (v) << off; \
	if (((bit) + (size) - 1) / 32 > i) { \
		_param_word(base, w)[i + 1] |= (v) >> (off ? (32 - off) : 0); \
	} \
}

#define ipu_ch_param_mod_field(base, w, bit, size, v) { \
	int i = (bit) / 32; \
	int off = (bit) % 32; \
	u32 mask = (1UL << size) - 1; \
	u32 temp = _param_word(base, w)[i]; \
	temp &= ~(mask << off); \
	_param_word(base, w)[i] = temp | (v) << off; \
	if (((bit) + (size) - 1) / 32 > i) { \
		temp = _param_word(base, w)[i + 1]; \
		temp &= ~(mask >> (32 - off)); \
		_param_word(base, w)[i + 1] = \
			temp | ((v) >> (off ? (32 - off) : 0)); \
	} \
}

#define ipu_ch_param_read_field(base, w, bit, size) ({ \
	u32 temp2; \
	int i = (bit) / 32; \
	int off = (bit) % 32; \
	u32 mask = (1UL << size) - 1; \
	u32 temp1 = _param_word(base, w)[i]; \
	temp1 = mask & (temp1 >> off); \
	if (((bit)+(size) - 1) / 32 > i) { \
		temp2 = _param_word(base, w)[i + 1]; \
		temp2 &= mask >> (off ? (32 - off) : 0); \
		temp1 |= temp2 << (off ? (32 - off) : 0); \
	} \
	temp1; \
})

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

static inline void ipu_ch_params_set_packing(struct ipu_ch_param *p,
					      int red_width, int red_offset,
					      int green_width, int green_offset,
					      int blue_width, int blue_offset,
					      int alpha_width, int alpha_offset) {
	/* Setup red width and offset */
	ipu_ch_param_set_field(p, 1, 116, 3, red_width - 1);
	ipu_ch_param_set_field(p, 1, 128, 5, red_offset);
	/* Setup green width and offset */
	ipu_ch_param_set_field(p, 1, 119, 3, green_width - 1);
	ipu_ch_param_set_field(p, 1, 133, 5, green_offset);
	/* Setup blue width and offset */
	ipu_ch_param_set_field(p, 1, 122, 3, blue_width - 1);
	ipu_ch_param_set_field(p, 1, 138, 5, blue_offset);
	/* Setup alpha width and offset */
	ipu_ch_param_set_field(p, 1, 125, 3, alpha_width - 1);
	ipu_ch_param_set_field(p, 1, 143, 5, alpha_offset);
}

int ipu_ch_write_params(struct fb_info *fbi) {
	struct ipu_ch_param params;

	memset(&params, 0, sizeof(params));

	/* RGB565 */
	ipu_ch_param_set_field(&params, 0, 107, 3, 3);	/* bits/pixel */
	ipu_ch_param_set_field(&params, 1, 85, 4, 7);	/* pix format */
	ipu_ch_param_set_field(&params, 1, 78, 7, 15);	/* burst size */

	ipu_ch_params_set_packing(&params, 5, 0, 6, 5, 5, 11, 8, 16);

	return 0;
}

int ipu_init_channel_buffer(struct fb_info *fbi) {
	ipu_ch_write_params(fbi);
	return 0;
}

static irq_return_t ipu_error_handler(unsigned int irq_nr, void *data) {
	log_error("IPU%d error interrupt request", (int) data);
	return IRQ_HANDLED;
}

static irq_return_t ipu_sync_handler(unsigned int irq_nr, void *data) {
	log_error("IPU%d sync interrupt request", (int) data);
	return IRQ_HANDLED;
}

int ipu_probe(void)
{
	int i, err;
	/* Enable IPU clock */

	/* Clear internal memories of IPU */
	REG32_STORE(IPU_MEM_RST, 0x807FFFFF);
	while (REG32_LOAD(IPU_MEM_RST) & 0x80000000);

	/* Init display controller mappings */

	/* Disable all interrupts */
	for (i = 1; i < 16; i++) {
		REG32_STORE(IPU_INT_CTRL(i), 0);
	}

	/* Enable error interrupts */
	REG32_STORE(IPU_INT_CTRL(5),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(6),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(9),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(10), 0xFFFFFFFF);

	if ((err = irq_attach(IPU1_ERROR_IRQ,
			ipu_error_handler, 0,
			(void *) 1, /* IPU1 */
			"IPU1 error interrupt request"))) {
		log_error("Failed to attach IPU1 error IRQ handler");
		return err;
	}

	if ((err = irq_attach(IPU2_ERROR_IRQ,
			ipu_error_handler, 0,
			(void *) 2, /* IPU2 */
			"IPU2 error interrupt request"))) {
		log_error("Failed to attach IPU2 error IRQ handler");
		return err;
	}

	if ((err = irq_attach(IPU1_SYNC_IRQ,
			ipu_sync_handler, 0,
			(void *) 1, /* IPU1 */
			"IPU1 sync interrupt request"))) {
		log_error("Failed to attach IPU1 sync IRQ handler");
		return err;
	}

	if ((err = irq_attach(IPU2_SYNC_IRQ,
			ipu_sync_handler, 0,
			(void *) 2, /* IPU2 */
			"IPU2 sync interrupt request"))) {
		log_error("Failed to attach IPU2 sync IRQ handler");
		return err;
	}

	/* Init DMFC */

	/* Setup priority */

	/* Disable IPU clock */
	return 0;
}

static struct periph_memory_desc ipu_mem = {
	.start = IPU_BASE,
	.len   = 0x100000,
};

PERIPH_MEMORY_DEFINE(ipu_mem);
