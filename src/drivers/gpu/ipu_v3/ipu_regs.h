/**
 * @file
 *
 * @data Sep 15, 2017
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_
#define SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_

#include <kernel/printk.h>

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

#define IPU_CPMEM		(IPU_CONF + 0x10000)

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

#define IPU_IDMAC_REG(offset)       (offset)

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
	uint32_t udge[16];
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

#endif /* SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_ */
