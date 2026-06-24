/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <kernel/kprintf.h>
#include <drivers/fb.h>

static unsigned int lcg_state = 123456789;

static unsigned int lcg32(void)
{
    lcg_state = lcg_state * 1664525 + 1013904223;
    return lcg_state;
}

static volatile pixel_t *fb;

void fb_init(void)
{
    pr_debug(
        "fb: Framebuffer at physical 0x%lx, %ux%ux%u, pitch: %u B.\n",
        fb_info.phys_addr,
        fb_info.width,
        fb_info.height,
        fb_info.bpp,
        fb_info.pitch
    );

    fb = (volatile pixel_t *)fb_info.virt_addr;

    // Test pattern.
    for (unsigned int y = 0; y < fb_info.height; y++)
    {
        for (unsigned int x = 0; x < fb_info.width; x++)
        {
            fb_set_pixel(x, y, rgba(255, 0, 0, lcg32() & 0xff));
        }
    }
}

void fb_set_pixel(unsigned int x, unsigned int y, pixel_t pixel)
{
    if (!fb)
    {
        return;
    }

    fb[y * (fb_info.pitch / 4) + x] = pixel;
}
