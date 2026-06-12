/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_FB_H
#define DRIVERS_FB_H

#include <torus/compiler.h>
#include <torus/types.h>

struct fb_info
{
    u64 phys_addr;
    u64 virt_addr;
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
} __packed;

extern struct fb_info fb_info;

typedef u32 pixel_t;

__always_inline __const pixel_t rgba(u8 r, u8 g, u8 b, u8 a)
{
    return ((pixel_t)a << 24) | ((pixel_t)b << 16) | ((pixel_t)g << 8) | (pixel_t)r;
}

extern void fb_init(void);
extern void fb_set_pixel(unsigned int x, unsigned int y, pixel_t pixel);

#endif // DRIVERS_FB_H
