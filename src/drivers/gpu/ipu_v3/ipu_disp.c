#include <errno.h>
#include <util/log.h>
#include "uboot_ipu_compat.h"
#include "ipu_regs.h"

enum csc_type_t {
	RGB2YUV = 0,
	YUV2RGB,
	RGB2RGB,
	YUV2YUV,
	CSC_NONE,
	CSC_NUM
};

struct dp_csc_param_t {
	int mode;
	const int (*coeff)[5][3];
};
#define SYNC_WAVE 0

/* DC display ID assignments */
#define DC_DISP_ID_SYNC(di)	(di)
#define DC_DISP_ID_SERIAL	2
#define DC_DISP_ID_ASYNC	3

/* DC microcode address */
#define DC_MCODE_DI0			0	/* 0 - 5 */
#define MCI_I			0
#define MCI_NL			1
#define MCI_EOL			2
#define MCI_NEW_DATA		3
#define MCI_EVEN_UGDE		4
#define MCI_ODD_UGDE		5

#define DC_MCODE_DI1			6	/* 6-11 */

#define DC_MCODE_ASYNC_NEW_DATA		0x64


int dmfc_type_setup;
static int dmfc_size_28, dmfc_size_29, dmfc_size_24, dmfc_size_27, dmfc_size_23;
int g_di1_tvout;

void ipu_dmfc_init(int dmfc_type, int first)
{
	u32 dmfc_wr_chan, dmfc_dp_chan;

	if (first) {
		if (dmfc_type_setup > dmfc_type)
			dmfc_type = dmfc_type_setup;
		else
			dmfc_type_setup = dmfc_type;

		/* disable DMFC-IC channel*/
		__raw_writel(0x2, DMFC_IC_CTRL);
	} else
		dmfc_type_setup = dmfc_type;

	/* 1 - segment 0, 1;
	 * 5B - segement 4, 5;
	 * 5F - segement 6, 7;
	 * 1C, 2C and 6B, 6F unused;
	 */
	debug("IPU DMFC NORMAL mode: 1(0~1), 5B(4,5), 5F(6,7)\n");
	dmfc_wr_chan = 0x00000090;
	dmfc_dp_chan = 0x00009694;
	dmfc_size_28 = 128 * 4;
	dmfc_size_29 = 0;
	dmfc_size_24 = 0;
	dmfc_size_27 = 128 * 4;
	dmfc_size_23 = 128 * 4;

	__raw_writel(dmfc_wr_chan, DMFC_WR_CHAN);
	__raw_writel(0x202020F6, DMFC_WR_CHAN_DEF);
	__raw_writel(dmfc_dp_chan, DMFC_DP_CHAN);
	/* Enable chan 5 watermark set at 5 bursts and clear at 7 bursts */
	__raw_writel(0x2020F6F6, DMFC_DP_CHAN_DEF);
}

void ipu_dmfc_set_wait4eot(int dma_chan, int width)
{
	u32 dmfc_gen1 = __raw_readl(DMFC_GENERAL1);

	if (dma_chan == 23) { /*5B*/
		if (dmfc_size_23 / width > 3)
			dmfc_gen1 |= 1UL << 20;
		else
			dmfc_gen1 &= ~(1UL << 20);
	} else if (dma_chan == 24) { /*6B*/
		if (dmfc_size_24 / width > 1)
			dmfc_gen1 |= 1UL << 22;
		else
			dmfc_gen1 &= ~(1UL << 22);
	} else if (dma_chan == 27) { /*5F*/
		if (dmfc_size_27 / width > 2)
			dmfc_gen1 |= 1UL << 21;
		else
			dmfc_gen1 &= ~(1UL << 21);
	} else if (dma_chan == 28) { /*1*/
		if (dmfc_size_28 / width > 2)
			dmfc_gen1 |= 1UL << 16;
		else
			dmfc_gen1 &= ~(1UL << 16);
	} else if (dma_chan == 29) { /*6F*/
		if (dmfc_size_29 / width > 1)
			dmfc_gen1 |= 1UL << 23;
		else
			dmfc_gen1 &= ~(1UL << 23);
	}

	__raw_writel(dmfc_gen1, DMFC_GENERAL1);
}

static void ipu_di_data_wave_config(int di,
				     int wave_gen,
				     int access_size, int component_size)
{
	u32 reg;
	reg = (access_size << DI_DW_GEN_ACCESS_SIZE_OFFSET) |
	    (component_size << DI_DW_GEN_COMPONENT_SIZE_OFFSET);
	__raw_writel(reg, DI_DW_GEN(di, wave_gen));
}

static void ipu_di_data_pin_config(int di, int wave_gen, int di_pin, int set,
				    int up, int down)
{
	u32 reg;

	reg = __raw_readl(DI_DW_GEN(di, wave_gen));
	reg &= ~(0x3 << (di_pin * 2));
	reg |= set << (di_pin * 2);
	__raw_writel(reg, DI_DW_GEN(di, wave_gen));

	__raw_writel((down << 16) | up, DI_DW_SET(di, wave_gen, set));
}

struct offset_mask {
	u8 offset;
	u8 mask;
};

struct f_mapping {
	struct offset_mask om[3];
};

enum {
	I_IPU_PIX_FMT_RGB24,
	I_IPU_PIX_FMT_BGR24,
	I_IPU_PIX_FMT_GBR24,
	I_IPU_PIX_FMT_RGB666,
	I_IPU_PIX_FMT_RGB565,
	I_IPU_PIX_FMT_LVDS666,
	I_IPU_PIX_FMT_YUV444,

	I_IPU_PIX_FMT_2CYCLE_START,
	I_IPU_PIX_FMT_VYUYa_1 = I_IPU_PIX_FMT_2CYCLE_START,
	I_IPU_PIX_FMT_VYUYa_2,
/* 16bit width for BT1120 */
	I_IPU_PIX_FMT_BT1120_1,
	I_IPU_PIX_FMT_BT1120_2,

	I_IPU_PIX_FMT_VYUY_1,
	I_IPU_PIX_FMT_VYUY_2,
	I_IPU_PIX_FMT_UYVY_1,
	I_IPU_PIX_FMT_UYVY_2,
	I_IPU_PIX_FMT_YUYV_1,
	I_IPU_PIX_FMT_YUYV_2,
	I_IPU_PIX_FMT_YVYU_1,
	I_IPU_PIX_FMT_YVYU_2,

/* 8bit width for BT656 */
	I_IPU_PIX_FMT_3CYCLE_START,
	I_IPU_PIX_FMT_BT656_1 = I_IPU_PIX_FMT_3CYCLE_START,
	I_IPU_PIX_FMT_BT656_2,
	I_IPU_PIX_FMT_BT656_3,
};

static struct f_mapping fmt_mappings[] = {
/* RGB formats are named from High bits to low bits */
/* YUV formats are named from low bits to high bits */
			/* B		G		R */
[I_IPU_PIX_FMT_RGB24] = {{{7, 0xFF}, {15, 0xFF}, {23, 0xFF}}},
[I_IPU_PIX_FMT_BGR24] = {{{23, 0xFF}, {15, 0xFF}, {7, 0xFF}}},
[I_IPU_PIX_FMT_GBR24] = {{{15, 0xFF}, {23, 0xFF}, {7, 0xFF}}},
[I_IPU_PIX_FMT_RGB666] = {{{5, 0xFC}, {11, 0xFC}, {17, 0xFC}}},
[I_IPU_PIX_FMT_RGB565] = {{{4, 0xF8}, {10, 0xFC}, {15, 0xF8}}},
[I_IPU_PIX_FMT_LVDS666] = {{{5, 0xFC}, {13, 0xFC}, {21, 0xFC}}},

			/* V		U		Y */
[I_IPU_PIX_FMT_YUV444] = {{{23, 0xFF}, {15, 0xFF}, {7, 0xFF}}},
#define BT656_IF_DI_MSB	23
[I_IPU_PIX_FMT_VYUYa_1] = {{{BT656_IF_DI_MSB - 8, 0xFF}, {0, 0x0}, {BT656_IF_DI_MSB, 0xFF}}},
[I_IPU_PIX_FMT_VYUYa_2] = {{{0, 0x0}, {BT656_IF_DI_MSB - 8, 0xFF}, {BT656_IF_DI_MSB, 0xFF}}},
/* 16bit width for BT1120 */
[I_IPU_PIX_FMT_BT1120_1] = {{{0, 0x0}, {BT656_IF_DI_MSB - 8, 0xFF}, {BT656_IF_DI_MSB, 0xFF}}},
[I_IPU_PIX_FMT_BT1120_2] = {{{BT656_IF_DI_MSB - 8, 0xFF}, {0, 0x0}, {BT656_IF_DI_MSB, 0xFF}}},

			/* V		U		Y */
[I_IPU_PIX_FMT_VYUY_1] = {{{7, 0xFF}, {0, 0x0}, {15, 0xFF}}},
[I_IPU_PIX_FMT_VYUY_2] = {{{0, 0x0}, {7, 0xFF}, {15, 0xFF}}},

[I_IPU_PIX_FMT_UYVY_1] = {{{0, 0x0}, {7, 0xFF}, {15, 0xFF}}},
[I_IPU_PIX_FMT_UYVY_2] = {{{7, 0xFF}, {0, 0x0}, {15, 0xFF}}},

[I_IPU_PIX_FMT_YUYV_1] = {{{0, 0x0}, {15, 0xFF}, {7, 0xFF}}},
[I_IPU_PIX_FMT_YUYV_2] = {{{15, 0xFF}, {0, 0x0}, {7, 0xFF}}},

[I_IPU_PIX_FMT_YVYU_1] = {{{15, 0xFF}, {0, 0x0}, {7, 0xFF}}},
[I_IPU_PIX_FMT_YVYU_2] = {{{0, 0x0}, {15, 0xFF}, {7, 0xFF}}},

/* 8bit width for BT656 */
[I_IPU_PIX_FMT_BT656_1] = {{{0, 0x0}, {BT656_IF_DI_MSB, 0xFF}, {0, 0x0}}},	/* U */
[I_IPU_PIX_FMT_BT656_2] = {{{0, 0x0}, {0, 0x0}, {BT656_IF_DI_MSB, 0xFF}}},	/* Y */
[I_IPU_PIX_FMT_BT656_3] = {{{BT656_IF_DI_MSB, 0xFF}, {0, 0x0}, {0, 0x0}}},	/* V */
};

#define DC_MAPPING_VAL_MAX	23
static unsigned long offset_mask_bitmap;

#define DC_MAPPING_PTR_MAX	29
static unsigned long mapping_bitmap;

static void set_bit_(int nr, volatile void *addr)
{
	int     mask;
	unsigned int *a = (unsigned int *)addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a |= mask;
}

static int find_field(u32 val, u32 *reg_base, unsigned long* bitmap, int max)
{
	int i = 0;
	u32 reg;

	while (i <= max) {
		if (!test_bit(i, bitmap))
			break;
		reg = __raw_readl(&reg_base[i >> 1]);
		if (val == (reg & 0xffff))
			return i;
		i++;
		if (!test_bit(i, bitmap))
			break;
		if (val == (reg >> 16))
			return i;
		i++;
	}
	i = ffz(*bitmap);

	if (i > max) {
		debug("out of mappings, max=%d\n", max);
		return -EINVAL;
	}

	reg = __raw_readl(&reg_base[i >> 1]);
	reg &= ~(0xFFFF << (16 * (i & 0x1)));
	reg |= val << (16 * (i & 0x1));
	__raw_writel(reg, &reg_base[i >> 1]);
	set_bit_(i, bitmap);
	debug("%s: [%d] = 0x%x max=%d\n", __func__, i, val, max);
	return i;
}

static int find_om(struct offset_mask *om)
{
	return find_field((om->offset << 8) | om->mask, DC_MAP_CONF_VAL(0), &offset_mask_bitmap, DC_MAPPING_VAL_MAX);
}

static int find_mptr(u32 val)
{
	return find_field(val, DC_MAP_CONF_PTR(0), &mapping_bitmap, DC_MAPPING_PTR_MAX);
}

int do_mapping(int i)
{
	struct f_mapping *fm = &fmt_mappings[i];
	u32 t = 0;

	for (i = 0; i < 3; i++) {
		int m = find_om(&fm->om[i]);

		if (m < 0)
			return m;
		t = (t >> 5) | (m << 10);
	}
	return find_mptr(t);
}

static int pixfmt_to_i(uint32_t fmt)
{
	switch (fmt) {
	case IPU_PIX_FMT_GENERIC:
	case IPU_PIX_FMT_RGB24:
		return  I_IPU_PIX_FMT_RGB24;
	case IPU_PIX_FMT_RGB666:
		return I_IPU_PIX_FMT_RGB666;
	case IPU_PIX_FMT_YUV444:
		return I_IPU_PIX_FMT_YUV444;
	case IPU_PIX_FMT_RGB565:
		return I_IPU_PIX_FMT_RGB565;
	case IPU_PIX_FMT_LVDS666:
		return I_IPU_PIX_FMT_LVDS666;
//	case IPU_PIX_FMT_VYUY:
//		return I_IPU_PIX_FMT_VYUY_1;
//	case IPU_PIX_FMT_BT1120:
//		return I_IPU_PIX_FMT_BT1120_1;
//	case IPU_PIX_FMT_BT656:
//		return I_IPU_PIX_FMT_BT656_1;
	case IPU_PIX_FMT_UYVY:
		return I_IPU_PIX_FMT_UYVY_1;
	case IPU_PIX_FMT_YUYV:
		return I_IPU_PIX_FMT_YUYV_1;
//	case IPU_PIX_FMT_YVYU:
//		return I_IPU_PIX_FMT_YVYU_1;
//	case IPU_PIX_FMT_GBR24:
//	case IPU_PIX_FMT_VYU444:
//		return I_IPU_PIX_FMT_GBR24;
	case IPU_PIX_FMT_BGR24:
		return I_IPU_PIX_FMT_BGR24;
	}
	return -EINVAL;
}



static int ipu_pixfmt_to_map(uint32_t fmt, int *mappings)
{
	int i = pixfmt_to_i(fmt);
	int ret;

	if (i < 0)
		return i;

	ret = do_mapping(i++);
	if (ret < 0)
		return ret;
	*mappings++ = ret;
	if (i > I_IPU_PIX_FMT_2CYCLE_START) {
		ret = do_mapping(i++);
		if (ret < 0)
			return ret;
		*mappings++ = ret;
		if (i > I_IPU_PIX_FMT_3CYCLE_START) {
			ret = do_mapping(i++);
			if (ret < 0)
				return ret;
			*mappings++ = ret;
		}
	}
	return 0;
}

static void ipu_di_sync_config(int di, int wave_gen,
				int run_count, int run_src,
				int offset_count, int offset_src,
				int repeat_count, int cnt_clr_src,
				int cnt_polarity_gen_en,
				int cnt_polarity_clr_src,
				int cnt_polarity_trigger_src,
				int cnt_up, int cnt_down)
{
	u32 reg;

	if ((run_count >= 0x1000) || (offset_count >= 0x1000) ||
		(repeat_count >= 0x1000) ||
		(cnt_up >= 0x400) || (cnt_down >= 0x400)) {
		debug("DI%d counters out of range.\n", di);
		return;
	}

	reg = (run_count << 19) | (++run_src << 16) |
	    (offset_count << 3) | ++offset_src;
	__raw_writel(reg, DI_SW_GEN0(di, wave_gen));
	reg = (cnt_polarity_gen_en << 29) | (++cnt_clr_src << 25) |
	    (++cnt_polarity_trigger_src << 12) | (++cnt_polarity_clr_src << 9);
	reg |= (cnt_down << 16) | cnt_up;
	if (repeat_count == 0) {
		/* Enable auto reload */
		reg |= 0x10000000;
	}
	__raw_writel(reg, DI_SW_GEN1(di, wave_gen));
	reg = __raw_readl(DI_STP_REP(di, wave_gen));
	reg &= ~(0xFFFF << (16 * ((wave_gen - 1) & 0x1)));
	reg |= repeat_count << (16 * ((wave_gen - 1) & 0x1));
	__raw_writel(reg, DI_STP_REP(di, wave_gen));
}

static void ipu_dc_write_tmpl(int word, u32 opcode, u32 operand, int map,
			       int wave, int glue, int sync)
{
	u32 reg;
	int stop = 1;

	reg = sync;
	reg |= (glue << 4);
	reg |= (++wave << 11);
	reg |= (++map << 15);
	reg |= (operand << 20) & 0xFFF00000;
	__raw_writel(reg, ipu_dc_tmpl_reg + word * 2);

	reg = (operand >> 12);
	reg |= opcode << 4;
	reg |= (stop << 9);
	__raw_writel(reg, ipu_dc_tmpl_reg + word * 2 + 1);
}

int32_t ipu_init_sync_panel(int disp, uint32_t pixel_clk,
			    uint16_t width, uint16_t height,
			    uint32_t pixel_fmt,
			    uint16_t h_start_width, uint16_t h_sync_width,
			    uint16_t h_end_width, uint16_t v_start_width,
			    uint16_t v_sync_width, uint16_t v_end_width,
			    uint32_t v_to_h_sync, ipu_di_signal_cfg_t sig)
{
	uint32_t reg;
	uint32_t di_gen, vsync_cnt;
	uint32_t div = 1;//, rounded_pixel_clk;
	uint32_t h_total, v_total;
	int map[3] = {0, 0, 0};
	int ret;
	//struct clk *di_parent;

	debug("panel size = %d x %d\n", width, height);

	if ((v_sync_width == 0) || (h_sync_width == 0))
		return -EINVAL;

	/* adapt panel to ipu restricitions */
	if (v_end_width < 2) {
		v_end_width = 2;
		debug("WARNING: v_end_width (lower_margin) must be >= 2, adjusted\n");
	}

	h_total = width + h_sync_width + h_start_width + h_end_width;
	v_total = height + v_sync_width + v_start_width + v_end_width;

	/* Init clocking */
	debug("pixel clk = %dHz\n", pixel_clk);
#if 0
	if (sig.ext_clk) {
		if (!(g_di1_tvout && (disp == 1))) { /*not round div for tvout*/
			/*
			 * Set the  PLL to be an even multiple
			 * of the pixel clock.
			 */
			if ((clk_get_usecount(g_pixel_clk[0]) == 0) &&
				(clk_get_usecount(g_pixel_clk[1]) == 0)) {
				di_parent = clk_get_parent(g_di_clk[disp]);
				rounded_pixel_clk =
					clk_round_rate(g_pixel_clk[disp],
						pixel_clk);
				div  = clk_get_rate(di_parent) /
					rounded_pixel_clk;
				if (div % 2)
					div++;
				if (clk_get_rate(di_parent) != div *
					rounded_pixel_clk)
					clk_set_rate(di_parent,
						div * rounded_pixel_clk);
				udelay(10000);
				clk_set_rate(g_di_clk[disp],
					2 * rounded_pixel_clk);
				udelay(10000);
			}
		}
		clk_set_parent(g_pixel_clk[disp], g_ldb_clk);
	} else {
		if (clk_get_usecount(g_pixel_clk[disp]) != 0)
			clk_set_parent(g_pixel_clk[disp], g_ipu_clk);
	}
	rounded_pixel_clk = clk_round_rate(g_pixel_clk[disp], pixel_clk);
	clk_set_rate(g_pixel_clk[disp], rounded_pixel_clk);
	udelay(5000);
	/* Get integer portion of divider */
	div = clk_get_rate(clk_get_parent(g_pixel_clk[disp])) /
		rounded_pixel_clk;
#endif

	ipu_di_data_wave_config(disp, SYNC_WAVE, div - 1, div - 1);
	ipu_di_data_pin_config(disp, SYNC_WAVE, DI_PIN15, 3, 0, div * 2);

	ret = ipu_pixfmt_to_map(pixel_fmt, map);
	if (ret < 0) {
		debug("IPU_DISP: No MAP\n");
		return ret;
	}

	di_gen = __raw_readl(DI_GENERAL(disp));

	if (sig.interlaced) {
		/* Setup internal HSYNC waveform */
		ipu_di_sync_config(
				disp,		/* display */
				1,		/* counter */
				h_total / 2 - 1,/* run count */
				DI_SYNC_CLK,	/* run_resolution */
				0,		/* offset */
				DI_SYNC_NONE,	/* offset resolution */
				0,		/* repeat count */
				DI_SYNC_NONE,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				0		/* COUNT DOWN */
				);

		/* Field 1 VSYNC waveform */
		ipu_di_sync_config(
				disp,		/* display */
				2,		/* counter */
				h_total - 1,	/* run count */
				DI_SYNC_CLK,	/* run_resolution */
				0,		/* offset */
				DI_SYNC_NONE,	/* offset resolution */
				0,		/* repeat count */
				DI_SYNC_NONE,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				4		/* COUNT DOWN */
				);

		/* Setup internal HSYNC waveform */
		ipu_di_sync_config(
				disp,		/* display */
				3,		/* counter */
				v_total * 2 - 1,/* run count */
				DI_SYNC_INT_HSYNC,	/* run_resolution */
				1,		/* offset */
				DI_SYNC_INT_HSYNC,	/* offset resolution */
				0,		/* repeat count */
				DI_SYNC_NONE,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				4		/* COUNT DOWN */
				);

		/* Active Field ? */
		ipu_di_sync_config(
				disp,		/* display */
				4,		/* counter */
				v_total / 2 - 1,/* run count */
				DI_SYNC_HSYNC,	/* run_resolution */
				v_start_width,	/*  offset */
				DI_SYNC_HSYNC,	/* offset resolution */
				2,		/* repeat count */
				DI_SYNC_VSYNC,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				0		/* COUNT DOWN */
				);

		/* Active Line */
		ipu_di_sync_config(
				disp,		/* display */
				5,		/* counter */
				0,		/* run count */
				DI_SYNC_HSYNC,	/* run_resolution */
				0,		/*  offset */
				DI_SYNC_NONE,	/* offset resolution */
				height / 2,	/* repeat count */
				4,		/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				0		/* COUNT DOWN */
				);

		/* Field 0 VSYNC waveform */
		ipu_di_sync_config(
				disp,		/* display */
				6,		/* counter */
				v_total - 1,	/* run count */
				DI_SYNC_HSYNC,	/* run_resolution */
				0,		/* offset */
				DI_SYNC_NONE,	/* offset resolution */
				0,		/* repeat count */
				DI_SYNC_NONE,	/* CNT_CLR_SEL  */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				0		/* COUNT DOWN */
				);

		/* DC VSYNC waveform */
		vsync_cnt = 7;
		ipu_di_sync_config(
				disp,		/* display */
				7,		/* counter */
				v_total / 2 - 1,/* run count */
				DI_SYNC_HSYNC,	/* run_resolution  */
				9,		/* offset  */
				DI_SYNC_HSYNC,	/* offset resolution */
				2,		/* repeat count */
				DI_SYNC_VSYNC,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				0		/* COUNT DOWN */
				);

		/* active pixel waveform */
		ipu_di_sync_config(
				disp,		/* display */
				8,		/* counter */
				0,		/* run count  */
				DI_SYNC_CLK,	/* run_resolution */
				h_start_width,	/* offset  */
				DI_SYNC_CLK,	/* offset resolution */
				width,		/* repeat count  */
				5,		/* CNT_CLR_SEL  */
				0,		/* CNT_POLARITY_GEN_EN  */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL  */
				0,		/* COUNT UP  */
				0		/* COUNT DOWN */
				);

		ipu_di_sync_config(
				disp,		/* display */
				9,		/* counter */
				v_total - 1,	/* run count */
				DI_SYNC_INT_HSYNC,/* run_resolution */
				v_total / 2,	/* offset  */
				DI_SYNC_INT_HSYNC,/* offset resolution  */
				0,		/* repeat count */
				DI_SYNC_HSYNC,	/* CNT_CLR_SEL */
				0,		/* CNT_POLARITY_GEN_EN  */
				DI_SYNC_NONE,	/* CNT_POLARITY_CLR_SEL  */
				DI_SYNC_NONE,	/* CNT_POLARITY_TRIGGER_SEL */
				0,		/* COUNT UP */
				4		/* COUNT DOWN */
				);

		/* set gentime select and tag sel */
		reg = __raw_readl(DI_SW_GEN1(disp, 9));
		reg &= 0x1FFFFFFF;
		reg |= (3 - 1)<<29 | 0x00008000;
		__raw_writel(reg, DI_SW_GEN1(disp, 9));

		__raw_writel(v_total / 2 - 1, DI_SCR_CONF(disp));

		/* set y_sel = 1 */
		di_gen |= 0x10000000;
		di_gen |= DI_GEN_POLARITY_5;
		di_gen |= DI_GEN_POLARITY_8;
	} else {
		int mc = disp ? DC_MCODE_DI1 : DC_MCODE_DI0;

		/* Setup internal HSYNC waveform */
		ipu_di_sync_config(disp, 1, h_total - 1, DI_SYNC_CLK,
				0, DI_SYNC_NONE, 0, DI_SYNC_NONE,
				0, DI_SYNC_NONE,
				DI_SYNC_NONE, 0, 0);

		/* Setup external (delayed) HSYNC waveform */
		ipu_di_sync_config(disp, DI_SYNC_HSYNC, h_total - 1,
				DI_SYNC_CLK, div * v_to_h_sync, DI_SYNC_CLK,
				0, DI_SYNC_NONE, 1, DI_SYNC_NONE,
				DI_SYNC_CLK, 0, h_sync_width * 2);
		/* Setup VSYNC waveform */
		vsync_cnt = DI_SYNC_VSYNC;
		ipu_di_sync_config(disp, DI_SYNC_VSYNC, v_total - 1,
				DI_SYNC_INT_HSYNC, 0, DI_SYNC_NONE, 0,
				DI_SYNC_NONE, 1, DI_SYNC_NONE,
				DI_SYNC_INT_HSYNC, 0, v_sync_width * 2);
		__raw_writel(v_total - 1, DI_SCR_CONF(disp));

		/* Setup active data waveform to sync with DC */
		ipu_di_sync_config(disp, 4, 0, DI_SYNC_HSYNC,
				v_sync_width + v_start_width, DI_SYNC_HSYNC,
				height,
				DI_SYNC_VSYNC, 0, DI_SYNC_NONE,
				DI_SYNC_NONE, 0, 0);
		ipu_di_sync_config(disp, 5, 0, DI_SYNC_CLK,
				h_sync_width + h_start_width, DI_SYNC_CLK,
				width, 4, 0, DI_SYNC_NONE, DI_SYNC_NONE, 0,
				0);

		/* reset all unused counters */
		__raw_writel(0, DI_SW_GEN0(disp, 6));
		__raw_writel(0, DI_SW_GEN1(disp, 6));
		__raw_writel(0, DI_SW_GEN0(disp, 7));
		__raw_writel(0, DI_SW_GEN1(disp, 7));
		__raw_writel(0, DI_SW_GEN0(disp, 8));
		__raw_writel(0, DI_SW_GEN1(disp, 8));
		__raw_writel(0, DI_SW_GEN0(disp, 9));
		__raw_writel(0, DI_SW_GEN1(disp, 9));

		reg = __raw_readl(DI_STP_REP(disp, 6));
		reg &= 0x0000FFFF;
		__raw_writel(reg, DI_STP_REP(disp, 6));
		__raw_writel(0, DI_STP_REP(disp, 7));
		__raw_writel(0, DI_STP_REP9(disp));

		/* Init template microcode */
		if ((pixel_fmt == IPU_PIX_FMT_YUYV) ||
		    (pixel_fmt == IPU_PIX_FMT_UYVY)) {
			debug("%s: map %d %d\n", __func__, map[0], map[1]);
			ipu_dc_write_tmpl(mc + MCI_EVEN_UGDE, WROD(0), 0, map[0], SYNC_WAVE, 0, 5);
			ipu_dc_write_tmpl(mc + MCI_ODD_UGDE, WROD(0), 0, map[1], SYNC_WAVE, 0, 5);
			/* configure user events according to DISP NUM */
			__raw_writel((width - 1), DC_UGDE_3(disp));
		}
		ipu_dc_write_tmpl(mc + MCI_NL, WROD(0), 0, map[0], SYNC_WAVE, 8, 5);
		ipu_dc_write_tmpl(mc + MCI_EOL, WROD(0), 0, map[0], SYNC_WAVE, 4, 5);
		ipu_dc_write_tmpl(mc + MCI_NEW_DATA, WROD(0), 0, map[0], SYNC_WAVE, 0, 5);

		if (sig.Hsync_pol)
			di_gen |= DI_GEN_POLARITY_2;
		if (sig.Vsync_pol)
			di_gen |= DI_GEN_POLARITY_3;

		if (!sig.clk_pol)
			di_gen |= DI_GEN_POL_CLK;

	}

	__raw_writel(di_gen, DI_GENERAL(disp));

	__raw_writel((--vsync_cnt << DI_VSYNC_SEL_OFFSET) |
			0x00000002, DI_SYNC_AS_GEN(disp));

	reg = __raw_readl(DI_POL(disp));
	reg &= ~(DI_POL_DRDY_DATA_POLARITY | DI_POL_DRDY_POLARITY_15);
	if (sig.enable_pol)
		reg |= DI_POL_DRDY_POLARITY_15;
	if (sig.data_pol)
		reg |= DI_POL_DRDY_DATA_POLARITY;
	__raw_writel(reg, DI_POL(disp));

	__raw_writel(width, DC_DISP_CONF2(DC_DISP_ID_SYNC(disp)));

	return 0;
}

void ipu_dp_dc_enable(ipu_channel_t channel) {
	int di;
	uint32_t reg;
	uint32_t dc_chan;

	if (channel == MEM_DC_SYNC)
		dc_chan = 1;
	else if ((channel == MEM_BG_SYNC) || (channel == MEM_FG_SYNC))
		dc_chan = 5;
	else
		return;

	if (channel == MEM_FG_SYNC) {
		/* Enable FG channel */
		reg = __raw_readl(DP_COM_CONF());
		__raw_writel(reg | DP_COM_CONF_FG_EN, DP_COM_CONF());

		reg = __raw_readl(IPU_SRM_PRI2) | 0x8;
		__raw_writel(reg, IPU_SRM_PRI2);
		return;
	}

	di = 0;// g_dc_di_assignment[dc_chan];

	/* Make sure other DC sync channel is not assigned same DI */
	reg = __raw_readl(DC_WR_CH_CONF(6 - dc_chan));
	if ((di << 2) == (reg & DC_WR_CH_CONF_PROG_DI_ID)) {
		reg &= ~DC_WR_CH_CONF_PROG_DI_ID;
		reg |= di ? 0 : DC_WR_CH_CONF_PROG_DI_ID;
		__raw_writel(reg, DC_WR_CH_CONF(6 - dc_chan));
	}

	reg = __raw_readl(DC_WR_CH_CONF(dc_chan));
	reg |= 4 << DC_WR_CH_CONF_PROG_TYPE_OFFSET;
	__raw_writel(reg, DC_WR_CH_CONF(dc_chan));

	u32 disp_gen = __raw_readl(IPU_DISP_GEN);
	disp_gen |= DI0_COUNTER_RELEASE;
	disp_gen |= DI1_COUNTER_RELEASE;
	__raw_writel(disp_gen, IPU_DISP_GEN);
}
