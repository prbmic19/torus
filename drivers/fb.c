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

volatile unsigned int *fb;

void fb_init(void)
{
    pr_debug(
        "fb: Framebuffer at 0x%lx, %ux%ux%u, pitch: %u B.\n",
        fb_info.phys_addr,
        fb_info.width,
        fb_info.height,
        fb_info.bpp,
        fb_info.pitch
    );

    fb = (volatile unsigned int *)fb_info.phys_addr;

    // Test pattern.
    for (unsigned int y = 0; y < fb_info.height; y++)
    {
        for (unsigned int x = 0; x < fb_info.width; x++)
        {
            fb[y * (fb_info.pitch / 4) + x] = (lcg32() & 0xff) | 0xff000000;
        }
    }
}
