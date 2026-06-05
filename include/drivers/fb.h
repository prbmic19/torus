/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_FB_H
#define DRIVERS_FB_H

#include <torus/types.h>

struct fb_info
{
    u64 phys_addr;
    u32 pitch;
    u32 width;
    u32 height;
    u32 bpp;
    u32 red_size;
    u32 red_shift;
    u32 green_size;
    u32 green_shift;
    u32 blue_size;
    u32 blue_shift;
    u32 rsvd_size;
    u32 rsvd_shift;
};

extern struct fb_info fb_info;
extern volatile unsigned int *fb;

extern void fb_init(void);

#endif // DRIVERS_FB_H
