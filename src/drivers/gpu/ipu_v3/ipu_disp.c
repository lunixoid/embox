#include <util/log.h>
#include "uboot_ipu_compat.h"
#include "ipu_regs.h"

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

