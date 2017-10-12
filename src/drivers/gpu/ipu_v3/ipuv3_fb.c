/**
 * @file ipuv3_fb.c
 * @brief Framebuffer for IPU (no regs/cloks/etc here)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.10.2017
 */

#include <embox/unit.h>
#include <drivers/video/fb.h>
#include <string.h>
#include "ipu_regs.h"

EMBOX_UNIT_INIT(ipu_init);

extern int ipu_setup_params();
extern int ipu_probe(void);
extern int ipu_init_channel(int ch);
extern int ipu_enable_channel(int ch);
extern int ipu_disable_channel(int ch);
//extern int ipu_init_sync_panel(int ch);
extern int ipu_init_channel_buffer(struct fb_info *fb);
extern int32_t ipu_init_sync_panel(int disp, uint32_t pixel_clk,
			    uint16_t width, uint16_t height,
			    uint32_t pixel_fmt,
			    uint16_t h_start_width, uint16_t h_sync_width,
			    uint16_t h_end_width, uint16_t v_start_width,
			    uint16_t v_sync_width, uint16_t v_end_width,
			    uint32_t v_to_h_sync, ipu_di_signal_cfg_t sig);
extern void ipu_dp_dc_enable(ipu_channel_t channel);

#define IPU_MAX_WIDTH	1024
#define IPU_MAX_HEIGHT	768

static uint16_t ipu_fb[IPU_MAX_WIDTH * IPU_MAX_HEIGHT]
			__attribute__ ((aligned (0x8)));


static int ipu_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	int ch = info->id; /* This can be wrong, we need hw id actually */
	ipu_di_signal_cfg_t tmp;
	ipu_disable_channel(ch);

	/* Phase one */
	ipu_init_channel(info->id);

	/* Setup signal config */

	ipu_init_sync_panel(0, 170, 1024, 1024, IPU_PIX_FMT_RGB565,
			0, 2 * 0, 1024, 0, 2 * 0, 1024, 0, tmp);

	/* Phase two */
	ipu_init_channel_buffer(info);

	ipu_enable_channel(ch);

	return 0;
}

static int ipu_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));
	var->xres = var->xres_virtual = IPU_MAX_WIDTH;
	var->yres = var->yres_virtual = IPU_MAX_HEIGHT;
	var->bits_per_pixel = 16;

	return 0;
}

static struct fb_ops ipu_fb_ops = {
	.fb_set_var   = ipu_set_var,
	.fb_get_var   = ipu_get_var,
};

int ipu_init(void)
{
	char *mmap_base = (void *) ipu_fb;
	size_t mmap_len = 2 * IPU_MAX_WIDTH * IPU_MAX_HEIGHT;
	struct fb_info *fbi;

	ipu_probe();

	ipu_setup_params();

	fbi = fb_create(&ipu_fb_ops, mmap_base, mmap_len);
	fbi->id = MEM_DC_SYNC;

	ipu_set_var(fbi, &fbi->var);

	for (int i = 0; i < IPU_MAX_WIDTH * IPU_MAX_HEIGHT; i++) {
		ipu_fb[i] = 0xFFFF;//0x0101 * (i % 256);
	}

	REG32_ORIN(IPU_SRM_PRI1, 0xFFFFFFFF);
	REG32_ORIN(IPU_SRM_PRI2, 0xFFFFFFFF);

	return 0;
}
