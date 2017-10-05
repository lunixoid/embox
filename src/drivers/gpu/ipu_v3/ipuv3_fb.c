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

extern int ipu_setup_params(int ch);
extern int ipu_probe(void);
extern int ipu_disable_channel(int ch);
extern int ipu_enable_channel(int ch);
extern int ipu_init_sync_panel(void);

#define IPU_MAX_WIDTH	1024
#define IPU_MAX_HEIGHT	1024

static uint16_t ipu_fb[IPU_MAX_WIDTH * IPU_MAX_HEIGHT]
			__attribute__ ((aligned (0x8)));

static int setup_disp_phase_one(struct fb_info *info) {
	return 0;
}

static int setup_disp_phase_one(struct fb_info *info) {
	return 0;
}

static int ipu_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	int ch = info->id; /* This can be wrong, we need hw id actually */

	ipu_disable_channel(ch);

	setup_disp_phase_one(info);

	/* Setup signal config */

	setup_disp_phase_two(info);
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

	ipu_set_var(fbi, &fbi->var);

	return 0;
}
