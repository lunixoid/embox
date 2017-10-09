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
	uint32_t mask = (1UL << size) - 1; \
	uint32_t temp = _param_word(base, w)[i]; \
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
	uint32_t temp2; \
	int i = (bit) / 32; \
	int off = (bit) % 32; \
	uint32_t mask = (1UL << size) - 1; \
	uint32_t temp1 = _param_word(base, w)[i]; \
	temp1 = mask & (temp1 >> off); \
	if (((bit)+(size) - 1) / 32 > i) { \
		temp2 = _param_word(base, w)[i + 1]; \
		temp2 &= mask >> (off ? (32 - off) : 0); \
		temp1 |= temp2 << (off ? (32 - off) : 0); \
	} \
	temp1; \
})

/* Default config      Parameters            Possible values */
static const int ipu_num = 1;             /* 1..2 */
static const int ipu_word_size_bits = 24; /* 8, 16, 24 or 32 */
static const int ipu_di = 0;              /* 0..1 */
static const int ipu_display = 2;         /* 0..3 */

static const int ch_num = 7;
static const int dma_ch = 28;

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

	REG32_STORE(IPU_DC_WR_CH_CONF(ch), conf);
	REG32_STORE(IPU_DC_WR_CH_ADDR(ch), 0);
	REG32_STORE(IPU_DC_GEN, 0x84);

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
	int width = fbi->var.xres;
	int height = fbi->var.yres;
	int stride = 0, u_offset = 0, v_offset = 0;
	memset(&params, 0, sizeof(params));

	ipu_ch_param_set_field(&params, 0, 125, 13, width - 1);
	ipu_ch_param_set_field(&params, 0, 138, 12, (height / 2) - 1);
	ipu_ch_param_set_field(&params, 1, 102, 14, (stride * 2) - 1);

	ipu_ch_param_set_field(&params, 1, 0, 29, ((uint32_t)fbi->screen_base) >> 3);
	ipu_ch_param_set_field(&params, 1, 29, 29, 0 >> 3);

	/* RGB565 */
	ipu_ch_param_set_field(&params, 0, 107, 3, 3);	/* bits/pixel */
	ipu_ch_param_set_field(&params, 1, 85, 4, 7);	/* pix format */
	ipu_ch_param_set_field(&params, 1, 78, 7, 15);	/* burst size */

	ipu_ch_params_set_packing(&params, 5, 0, 6, 5, 5, 11, 8, 16);

	ipu_ch_param_set_field(&params, 0, 46, 22, u_offset / 8);
	ipu_ch_param_set_field(&params, 0, 68, 22, v_offset / 8);

	memcpy(ipu_ch_param_addr(ch_num), &params, sizeof(params));
	return 0;
}

int ipu_init_channel_buffer(struct fb_info *fbi) {
	//uint32_t reg;
	ipu_ch_write_params(fbi);

	//reg = REG32_LOAD(IPU_CHA_DB_MODE_SEL(dma_ch));


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

static void ipu_dc_map_config(int map, int byte_num, int offset, int mask)
{
	int ptr = map * 3 + byte_num;
	uint32_t reg;

	reg = REG32_LOAD(DC_MAP_CONF_VAL(ptr));
	reg &= ~(0xFFFF << (16 * (ptr & 0x1)));
	reg |= ((offset << 8) | mask) << (16 * (ptr & 0x1));
	REG32_STORE(DC_MAP_CONF_VAL(ptr), reg);

	reg = REG32_LOAD(DC_MAP_CONF_PTR(map));
	reg &= ~(0x1F << ((16 * (map & 0x1)) + (5 * byte_num)));
	reg |= ptr << ((16 * (map & 0x1)) + (5 * byte_num));
	REG32_STORE(DC_MAP_CONF_PTR(map), reg);
}

static void ipu_dc_map_clear(int map)
{
	uint32_t reg = REG32_LOAD(DC_MAP_CONF_PTR(map));
	REG32_STORE(DC_MAP_CONF_PTR(map), reg & ~(0xFFFF << (16 * (map & 0x1))));
}

void ipu_init_dc_mappings(void)
{
	/* IPU_PIX_FMT_RGB24 */
	ipu_dc_map_clear(0);
	ipu_dc_map_config(0, 0, 7, 0xFF);
	ipu_dc_map_config(0, 1, 15, 0xFF);
	ipu_dc_map_config(0, 2, 23, 0xFF);

	/* IPU_PIX_FMT_RGB666 */
	ipu_dc_map_clear(1);
	ipu_dc_map_config(1, 0, 5, 0xFC);
	ipu_dc_map_config(1, 1, 11, 0xFC);
	ipu_dc_map_config(1, 2, 17, 0xFC);

	/* IPU_PIX_FMT_YUV444 */
	ipu_dc_map_clear(2);
	ipu_dc_map_config(2, 0, 15, 0xFF);
	ipu_dc_map_config(2, 1, 23, 0xFF);
	ipu_dc_map_config(2, 2, 7, 0xFF);

	/* IPU_PIX_FMT_RGB565 */
	ipu_dc_map_clear(3);
	ipu_dc_map_config(3, 0, 4, 0xF8);
	ipu_dc_map_config(3, 1, 10, 0xFC);
	ipu_dc_map_config(3, 2, 15, 0xF8);

	/* IPU_PIX_FMT_LVDS666 */
	ipu_dc_map_clear(4);
	ipu_dc_map_config(4, 0, 5, 0xFC);
	ipu_dc_map_config(4, 1, 13, 0xFC);
	ipu_dc_map_config(4, 2, 21, 0xFC);
}

static int dmfc_size_28, dmfc_size_29, dmfc_size_24, dmfc_size_27, dmfc_size_23;
void ipu_dmfc_init() {
	uint32_t dmfc_wr_chan, dmfc_dp_chan;

	/* disable DMFC-IC channel*/
	REG32_STORE(DMFC_IC_CTRL, 0x2);
	dmfc_wr_chan = 0x00000090;
	dmfc_dp_chan = 0x00009694;
	dmfc_size_28 = 128 * 4;
	dmfc_size_29 = 0;
	dmfc_size_24 = 0;
	dmfc_size_27 = 128 * 4;
	dmfc_size_23 = 128 * 4;

	REG32_STORE(DMFC_WR_CHAN, dmfc_wr_chan);
	REG32_STORE(DMFC_WR_CHAN_DEF, 0x202020F6);
	REG32_STORE(DMFC_DP_CHAN, dmfc_dp_chan);
	REG32_STORE(DMFC_DP_CHAN_DEF, 0x2020F6F6);
}

#define DCIC1_BASE 0x20E4000
#define DCIC2_BASE 0x20E8000

extern int clk_enable(char *clk_name);
int ipu_probe(void)
{
	int i, err;
	static char *clk_ipu_name = "ipu_";
	static char *clk_di_name  = "ipu__di_";
	/* Enable IPU and DI clocks */
	clk_ipu_name[3] = clk_di_name[3] = '0' + (char) ipu_num;
	clk_di_name[7] = '0' + (char) ipu_di;

	clk_enable(clk_ipu_name);
	clk_enable(clk_di_name);

	/* Turn off integrity check */
	clk_enable("dcic1"); /* Enable clk to write config register */
	REG32_CLEAR(DCIC1_BASE, 1);
	clk_enable("dcic2");
	//REG32_CLEAR(DCIC2_BASE, 1);

	/* Clear internal memories of IPU */
	REG32_STORE(IPU_MEM_RST, 0x807FFFFF);
	while (REG32_LOAD(IPU_MEM_RST) & 0x80000000) {
		log_debug("mem reset %x", REG32_LOAD(IPU_MEM_RST));
	}

	/* Init display controller mappings */
	ipu_init_dc_mappings();

	/* Disable all interrupts */
	for (i = 1; i < 16; i++) {
		REG32_STORE(IPU_INT_CTRL(i), 0);
	}

	/* Enable error interrupts */
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
	REG32_STORE(IPU_INT_CTRL(5),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(6),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(9),  0xFFFFFFFF);
	REG32_STORE(IPU_INT_CTRL(10), 0xFFFFFFFF);

	/* Init DMFC */
	ipu_dmfc_init();

	/* Setup priority */

	/* Disable IPU clock */
	return 0;
}

static struct periph_memory_desc ipu_mem = {
	.start = IPU_BASE,
	.len   = 0x100000,
};

PERIPH_MEMORY_DEFINE(ipu_mem);

static struct periph_memory_desc dcic1_mem = {
	.start = DCIC1_BASE,
	.len   = 0x20,
};

PERIPH_MEMORY_DEFINE(dcic1_mem);

static struct periph_memory_desc dcic2_mem = {
	.start = DCIC2_BASE,
	.len   = 0x20,
};

PERIPH_MEMORY_DEFINE(dcic2_mem);
