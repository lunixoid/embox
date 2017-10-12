/**
 * @file
 *
 * @data Sep 15, 2017
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_
#define SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_

#include <kernel/printk.h>
#include "uboot_ipu_compat.h"

#define IPU1_ERROR_IRQ  37
#define IPU1_SYNC_IRQ   38
#define IPU2_ERROR_IRQ  39
#define IPU2_SYNC_IRQ   40

#define IPU_BASE OPTION_MODULE_GET( \
		embox__driver__gpu__ipu_v3, \
		NUMBER,   \
		base_addr)

#define IPU_MCU_T_DEFAULT    8
#define IPU_CM_IDMAC_REG_OFS 0x00008000
#define IPU_CM_IC_REG_OFS    0x00020000
#define IPU_CM_IRT_REG_OFS   0x00028000
#define IPU_CM_CSI0_REG_OFS  0x00030000
#define IPU_CM_CSI1_REG_OFS  0x00038000
#define IPU_CM_SMFC_REG_OFS  0x00050000
#define IPU_CM_DC_REG_OFS    0x00058000
#define IPU_CM_DMFC_REG_OFS  0x00060000

#define IPU_CM_REG(offset)     (IPU_BASE + offset)

#define IPU_CONF               IPU_CM_REG(0)

#define IPU_CPMEM		(IPU_CONF + 0x100000)

#define IPU_INT_CTRL(n)             IPU_CM_REG(0x003c + (n - 1) * 4)
#define IPU_SRM_PRI1                IPU_CM_REG(0x00a0)
#define IPU_SRM_PRI2                IPU_CM_REG(0x00a4)
#define IPU_FS_PROC_FLOW1           IPU_CM_REG(0x00a8)
#define IPU_FS_PROC_FLOW2           IPU_CM_REG(0x00ac)
#define IPU_FS_PROC_FLOW3           IPU_CM_REG(0x00b0)
#define IPU_FS_DISP_FLOW1           IPU_CM_REG(0x00b4)
#define IPU_FS_DISP_FLOW2           IPU_CM_REG(0x00b8)
#define IPU_SKIP                    IPU_CM_REG(0x00bc)
#define IPU_DISP_ALT_CONF           IPU_CM_REG(0x00c0)
#define IPU_DISP_GEN                IPU_CM_REG(0x00c4)
#define IPU_DISP_ALT1               IPU_CM_REG(0x00c8)
#define IPU_DISP_ALT2               IPU_CM_REG(0x00cc)
#define IPU_DISP_ALT3               IPU_CM_REG(0x00d0)
#define IPU_DISP_ALT4               IPU_CM_REG(0x00d4)
#define IPU_SNOOP                   IPU_CM_REG(0x00d8)
#define IPU_MEM_RST                 IPU_CM_REG(0x00dc)
#define IPU_PM                      IPU_CM_REG(0x00e0)
#define IPU_GPR                     IPU_CM_REG(0x00e4)
#define IPU_CHA_DB_MODE_SEL(ch)     IPU_CM_REG(0x0150 + 4 * ((ch) / 32))
#define IPU_ALT_CHA_DB_MODE_SEL(ch) IPU_CM_REG(0x0168 + 4 * ((ch) / 32))
#define IPU_CHA_CUR_BUF(ch)         IPU_CM_REG(0x023C + 4 * ((ch) / 32))
#define IPU_ALT_CUR_BUF0            IPU_CM_REG(0x0244)
#define IPU_ALT_CUR_BUF1            IPU_CM_REG(0x0248)
#define IPU_SRM_STAT                IPU_CM_REG(0x024C)
#define IPU_PROC_TASK_STAT          IPU_CM_REG(0x0250)
#define IPU_DISP_TASK_STAT          IPU_CM_REG(0x0254)
#define IPU_CHA_BUF0_RDY(ch)        IPU_CM_REG(0x0268 + 4 * ((ch) / 32))
#define IPU_CHA_BUF1_RDY(ch)        IPU_CM_REG(0x0270 + 4 * ((ch) / 32))
#define IPU_CHA_BUF2_RDY(ch)        IPU_CM_REG(0x0288 + 4 * ((ch) / 32))
#define IPU_ALT_CHA_BUF0_RDY(ch)    IPU_CM_REG(0x0278 + 4 * ((ch) / 32))
#define IPU_ALT_CHA_BUF1_RDY(ch)    IPU_CM_REG(0x0280 + 4 * ((ch) / 32))

#define IPU_INT_STAT(n)             IPU_CM_REG(0x0200 + 4 * (n))

#define IPU_DI0_COUNTER_RELEASE     (1 << 24)
#define IPU_DI1_COUNTER_RELEASE     (1 << 25)

#define IPU_IDMAC_REG(offset)       (offset + IPU_BASE + 0x8000)

#define IDMAC_CONF                  IPU_IDMAC_REG(0x0000)
#define IDMAC_CHA_EN(ch)            IPU_IDMAC_REG(0x0004 + 4 * ((ch) / 32))
#define IDMAC_SEP_ALPHA             IPU_IDMAC_REG(0x000c)
#define IDMAC_ALT_SEP_ALPHA         IPU_IDMAC_REG(0x0010)
#define IDMAC_CHA_PRI(ch)           IPU_IDMAC_REG(0x0014 + 4 * ((ch) / 32))
#define IDMAC_WM_EN(ch)             IPU_IDMAC_REG(0x001c + 4 * ((ch) / 32))
#define IDMAC_CH_LOCK_EN_1          IPU_IDMAC_REG(0x0024)
#define IDMAC_CH_LOCK_EN_2          IPU_IDMAC_REG(0x0028)
#define IDMAC_SUB_ADDR_0            IPU_IDMAC_REG(0x002c)
#define IDMAC_SUB_ADDR_1            IPU_IDMAC_REG(0x0030)
#define IDMAC_SUB_ADDR_2            IPU_IDMAC_REG(0x0034)
#define IDMAC_BAND_EN(ch)           IPU_IDMAC_REG(0x0040 + 4 * ((ch) / 32))
#define IDMAC_CHA_BUSY(ch)          IPU_IDMAC_REG(0x0100 + 4 * ((ch) / 32))

static inline uint32_t IPU_DC_WR_CH_CONF(int n) {
	switch (n) {
	case 1:
		return IPU_CM_REG(0x5801C);
	case 2:
		return IPU_CM_REG(0x58038);
	case 5:
		return IPU_CM_REG(0x5805C);
	case 6:
		return IPU_CM_REG(0x58078);
	case 8:
		return IPU_CM_REG(0x58094);
	case 9:
		return IPU_CM_REG(0x580B4);
	}

	printk("IPU: bad channel id=%d\n", n);
	return 0;
}

#define IPU_DC_WR_CH_ADDR(n) (IPU_DC_WR_CH_CONF(n) + 4)

#define IPU_DC_GEN		IPU_CM_REG(0x580D4)

#define DC_REG			((struct ipu_dc *)(IPU_BASE + IPU_CM_DC_REG_OFS))
#define DC_MAP_CONF_PTR(n)	(&DC_REG->dc_map_ptr[n / 2])
#define DC_MAP_CONF_VAL(n)	(&DC_REG->dc_map_val[n / 2])

struct ipu_dc_ch {
	uint32_t wr_ch_conf;
	uint32_t wr_ch_addr;
	uint32_t rl[5];
};

struct ipu_dc {
	struct ipu_dc_ch dc_ch0_1_2[3];
	uint32_t cmd_ch_conf_3;
	uint32_t cmd_ch_conf_4;
	struct ipu_dc_ch dc_ch5_6[2];
	struct ipu_dc_ch dc_ch8;
	uint32_t rl6_ch_8;
	struct ipu_dc_ch dc_ch9;
	uint32_t rl6_ch_9;
	uint32_t gen;
	uint32_t disp_conf1[4];
	uint32_t disp_conf2[4];
	uint32_t di0_conf[2];
	uint32_t di1_conf[2];
	uint32_t dc_map_ptr[15];
	uint32_t dc_map_val[12];
	uint32_t ugde[16];
	uint32_t lla[2];
	uint32_t r_lla[2];
	uint32_t wr_ch_addr_5_alt;
	uint32_t stat;
};

#define DMFC_RD_CHAN		(IPU_BASE + 0x60000)
#define DMFC_WR_CHAN		(IPU_BASE + 0x60004)
#define DMFC_WR_CHAN_DEF	(IPU_BASE + 0x60008)
#define DMFC_DP_CHAN		(IPU_BASE + 0x6000C)
#define DMFC_DP_CHAN_DEF	(IPU_BASE + 0x60010)
#define DMFC_GENERAL_1		(IPU_BASE + 0x60014)
#define DMFC_GENERAL_2		(IPU_BASE + 0x60018)
#define DMFC_IC_CTRL		(IPU_BASE + 0x6001C)

#define IPU_MAX_CH	32
#define _MAKE_CHAN(num, v_in, g_in, a_in, out) \
	((num << 24) | (v_in << 18) | (g_in << 12) | (a_in << 6) | out)
#define _MAKE_ALT_CHAN(ch)		(ch | (IPU_MAX_CH << 24))
#define IPU_CHAN_ID(ch)			(ch >> 24)
#define IPU_CHAN_ALT(ch)		(ch & 0x02000000)
#define IPU_CHAN_ALPHA_IN_DMA(ch)	((uint32_t) (ch >> 6) & 0x3F)
#define IPU_CHAN_GRAPH_IN_DMA(ch)	((uint32_t) (ch >> 12) & 0x3F)
#define IPU_CHAN_VIDEO_IN_DMA(ch)	((uint32_t) (ch >> 18) & 0x3F)
#define IPU_CHAN_OUT_DMA(ch)		((uint32_t) (ch & 0x3F))
#define NO_DMA 0x3F
#define ALT	1

typedef enum {
	CHAN_NONE = -1,

	MEM_DC_SYNC = _MAKE_CHAN(7, 28, NO_DMA, NO_DMA, NO_DMA),
	MEM_DC_ASYNC = _MAKE_CHAN(8, 41, NO_DMA, NO_DMA, NO_DMA),
	MEM_BG_SYNC = _MAKE_CHAN(9, 23, NO_DMA, 51, NO_DMA),
	MEM_FG_SYNC = _MAKE_CHAN(10, 27, NO_DMA, 31, NO_DMA),

	MEM_BG_ASYNC0 = _MAKE_CHAN(11, 24, NO_DMA, 52, NO_DMA),
	MEM_FG_ASYNC0 = _MAKE_CHAN(12, 29, NO_DMA, 33, NO_DMA),
	MEM_BG_ASYNC1 = _MAKE_ALT_CHAN(MEM_BG_ASYNC0),
	MEM_FG_ASYNC1 = _MAKE_ALT_CHAN(MEM_FG_ASYNC0),

	DIRECT_ASYNC0 = _MAKE_CHAN(13, NO_DMA, NO_DMA, NO_DMA, NO_DMA),
	DIRECT_ASYNC1 = _MAKE_CHAN(14, NO_DMA, NO_DMA, NO_DMA, NO_DMA),

} ipu_channel_t;

/*
 * Enumeration of types of buffers for a logical channel.
 */
typedef enum {
	IPU_OUTPUT_BUFFER = 0,	/*< Buffer for output from IPU */
	IPU_ALPHA_IN_BUFFER = 1,	/*< Buffer for input to IPU */
	IPU_GRAPH_IN_BUFFER = 2,	/*< Buffer for input to IPU */
	IPU_VIDEO_IN_BUFFER = 3,	/*< Buffer for input to IPU */
	IPU_INPUT_BUFFER = IPU_VIDEO_IN_BUFFER,
	IPU_SEC_INPUT_BUFFER = IPU_GRAPH_IN_BUFFER,
} ipu_buffer_t;

#define IPU_PANEL_SERIAL		1
#define IPU_PANEL_PARALLEL		2

struct ipu_channel {
	u8 video_in_dma;
	u8 alpha_in_dma;
	u8 graph_in_dma;
	u8 out_dma;
};

#define IPU_DI0_REG_BASE	0x00040000
#define IPU_DI1_REG_BASE	0x00048000
#define DI_REG(di)		((struct ipu_di *)(IPU_BASE + \
				((di == 1) ? IPU_DI1_REG_BASE : \
				IPU_DI0_REG_BASE)))
#define DI_GENERAL(di)		(&DI_REG(di)->general)
#define DI_BS_CLKGEN0(di)	(&DI_REG(di)->bs_clkgen0)
#define DI_BS_CLKGEN1(di)	(&DI_REG(di)->bs_clkgen1)

#define DI_SW_GEN0(di, gen)	(&DI_REG(di)->sw_gen0[gen - 1])
#define DI_SW_GEN1(di, gen)	(&DI_REG(di)->sw_gen1[gen - 1])
#define DI_STP_REP(di, gen)	(&DI_REG(di)->stp_rep[(gen - 1) / 2])
#define DI_STP_REP9(di)		(&DI_REG(di)->stp_rep9)
#define DI_SYNC_AS_GEN(di)	(&DI_REG(di)->sync_as)
#define DI_DW_GEN(di, gen)	(&DI_REG(di)->dw_gen[gen])
#define DI_DW_SET(di, gen, set)	(&DI_REG(di)->dw_set[gen + 12 * set])
#define DI_POL(di)		(&DI_REG(di)->pol)
#define DI_SCR_CONF(di)		(&DI_REG(di)->scr_conf)

struct ipu_di {
	u32 general;
	u32 bs_clkgen0;
	u32 bs_clkgen1;
	u32 sw_gen0[9];
	u32 sw_gen1[9];
	u32 sync_as;
	u32 dw_gen[12];
	u32 dw_set[48];
	u32 stp_rep[4];
	u32 stp_rep9;
	u32 ser_conf;
	u32 ssc;
	u32 pol;
	u32 aw0;
	u32 aw1;
	u32 scr_conf;
	u32 stat;
};

#define IPU_DC_TMPL_REG_BASE	0x00180000

#define ipu_dc_tmpl_reg ((uint32_t *) IPU_BASE + IPU_DC_TMPL_REG_BASE)

typedef struct {
	unsigned datamask_en:1;
	unsigned ext_clk:1;
	unsigned interlaced:1;
	unsigned odd_field_first:1;
	unsigned clksel_en:1;
	unsigned clkidle_en:1;
	unsigned data_pol:1;	/* true = inverted */
	unsigned clk_pol:1;	/* true = rising edge */
	unsigned enable_pol:1;
	unsigned Hsync_pol:1;	/* true = active high */
	unsigned Vsync_pol:1;
} ipu_di_signal_cfg_t;

enum di_pins {
	DI_PIN11 = 0,
	DI_PIN12 = 1,
	DI_PIN13 = 2,
	DI_PIN14 = 3,
	DI_PIN15 = 4,
	DI_PIN16 = 5,
	DI_PIN17 = 6,
	DI_PIN_CS = 7,

	DI_PIN_SER_CLK = 0,
	DI_PIN_SER_RS = 1,
};

enum di_sync_wave {
	DI_SYNC_NONE = -1,
	DI_SYNC_CLK = 0,
	DI_SYNC_INT_HSYNC = 1,
	DI_SYNC_HSYNC = 2,
	DI_SYNC_VSYNC = 3,
	DI_SYNC_DE = 5,
};

enum {
	IPU_CONF_DP_EN = 0x00000020,
	IPU_CONF_DI0_EN = 0x00000040,
	IPU_CONF_DI1_EN = 0x00000080,
	IPU_CONF_DMFC_EN = 0x00000400,
	IPU_CONF_DC_EN = 0x00000200,

	DI0_COUNTER_RELEASE = 0x01000000,
	DI1_COUNTER_RELEASE = 0x02000000,

	DI_DW_GEN_ACCESS_SIZE_OFFSET = 24,
	DI_DW_GEN_COMPONENT_SIZE_OFFSET = 16,

	DI_GEN_DI_CLK_EXT = 0x100000,
	DI_GEN_POLARITY_1 = 0x00000001,
	DI_GEN_POLARITY_2 = 0x00000002,
	DI_GEN_POLARITY_3 = 0x00000004,
	DI_GEN_POLARITY_4 = 0x00000008,
	DI_GEN_POLARITY_5 = 0x00000010,
	DI_GEN_POLARITY_6 = 0x00000020,
	DI_GEN_POLARITY_7 = 0x00000040,
	DI_GEN_POLARITY_8 = 0x00000080,
	DI_GEN_POL_CLK = 0x20000,

	DI_POL_DRDY_DATA_POLARITY = 0x00000080,
	DI_POL_DRDY_POLARITY_15 = 0x00000010,
	DI_VSYNC_SEL_OFFSET = 13,

	DC_WR_CH_CONF_FIELD_MODE = 0x00000200,
	DC_WR_CH_CONF_PROG_TYPE_OFFSET = 5,
	DC_WR_CH_CONF_PROG_TYPE_MASK = 0x000000E0,
	DC_WR_CH_CONF_PROG_DI_ID = 0x00000004,
	DC_WR_CH_CONF_PROG_DISP_ID_OFFSET = 3,
	DC_WR_CH_CONF_PROG_DISP_ID_MASK = 0x00000018,

	DP_COM_CONF_FG_EN = 0x00000001,
	DP_COM_CONF_GWSEL = 0x00000002,
	DP_COM_CONF_GWAM = 0x00000004,
	DP_COM_CONF_GWCKE = 0x00000008,
	DP_COM_CONF_CSC_DEF_MASK = 0x00000300,
	DP_COM_CONF_CSC_DEF_OFFSET = 8,
	DP_COM_CONF_CSC_DEF_FG = 0x00000300,
	DP_COM_CONF_CSC_DEF_BG = 0x00000200,
	DP_COM_CONF_CSC_DEF_BOTH = 0x00000100,
	DP_COM_CONF_GAMMA_EN = 0x00001000,
	DP_COM_CONF_GAMMA_YUV_EN = 0x00002000,
};

#define DC_GEN			(&DC_REG->gen)
#define DC_DISP_CONF2(disp)	(&DC_REG->disp_conf2[disp])
#define DC_STAT			(&DC_REG->stat)
#define DC_UGDE_0(evt)		(&DC_REG->ugde[0 + (4 * (evt))])
#define DC_UGDE_1(evt)		(&DC_REG->ugde[1 + (4 * (evt))])
#define DC_UGDE_2(evt)		(&DC_REG->ugde[2 + (4 * (evt))])
#define DC_UGDE_3(evt)		(&DC_REG->ugde[3 + (4 * (evt))])

#define DP_SYNC 0
#define DP_ASYNC0 0x60
#define DP_ASYNC1 0xBC

struct ipu_com_async {
	u32 com_conf_async;
	u32 graph_wind_ctrl_async;
	u32 fg_pos_async;
	u32 cur_pos_async;
	u32 cur_map_async;
	u32 gamma_c_async[8];
	u32 gamma_s_async[4];
	u32 dp_csca_async[4];
	u32 dp_csc_async[2];
};

struct ipu_dp {
	u32 com_conf_sync;
	u32 graph_wind_ctrl_sync;
	u32 fg_pos_sync;
	u32 cur_pos_sync;
	u32 cur_map_sync;
	u32 gamma_c_sync[8];
	u32 gamma_s_sync[4];
	u32 csca_sync[4];
	u32 csc_sync[2];
	u32 cur_pos_alt;
	struct ipu_com_async async[2];
};


#define IPU_DP_REG_BASE		0x00018000
#define DP_REG			((struct ipu_dp *)(IPU_BASE + \
				IPU_DP_REG_BASE))
#define DP_COM_CONF()		(&DP_REG->com_conf_sync)
#define DP_GRAPH_WIND_CTRL()	(&DP_REG->graph_wind_ctrl_sync)
#define DP_CSC_A_0()		(&DP_REG->csca_sync[0])
#define DP_CSC_A_1()		(&DP_REG->csca_sync[1])
#define DP_CSC_A_2()		(&DP_REG->csca_sync[2])
#define DP_CSC_A_3()		(&DP_REG->csca_sync[3])

#define DP_CSC_0()		(&DP_REG->csc_sync[0])
#define DP_CSC_1()		(&DP_REG->csc_sync[1])

/* DC template opcodes */
#define WROD(lf)		(0x18 | (lf << 1))

#define DC_WR_CH_CONF(n) IPU_DC_WR_CH_CONF(n)

#endif /* SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_ */
