/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_FB_H
#define DRIVERS_FB_H

#include <torus/compiler.h>
#include <torus/types.h>
#include <kernel/build_bug.h>

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

static_assert(sizeof(struct fb_info) == 64, "Size of struct fb_info has changed.");
ASSERT_STRUCT_OFFSET(struct fb_info, phys_addr, 0);
ASSERT_STRUCT_OFFSET(struct fb_info, virt_addr, 8);
ASSERT_STRUCT_OFFSET(struct fb_info, pitch, 16);
ASSERT_STRUCT_OFFSET(struct fb_info, width, 20);
ASSERT_STRUCT_OFFSET(struct fb_info, height, 24);
ASSERT_STRUCT_OFFSET(struct fb_info, bpp, 28);
ASSERT_STRUCT_OFFSET(struct fb_info, red_size, 32);
ASSERT_STRUCT_OFFSET(struct fb_info, red_shift, 36);
ASSERT_STRUCT_OFFSET(struct fb_info, green_size, 40);
ASSERT_STRUCT_OFFSET(struct fb_info, green_shift, 44);
ASSERT_STRUCT_OFFSET(struct fb_info, blue_size, 48);
ASSERT_STRUCT_OFFSET(struct fb_info, blue_shift, 52);
ASSERT_STRUCT_OFFSET(struct fb_info, rsvd_size, 56);
ASSERT_STRUCT_OFFSET(struct fb_info, rsvd_shift, 60);

extern struct fb_info fb_info;

typedef u32 pixel_t;

__always_inline __const static pixel_t rgba(u8 r, u8 g, u8 b, u8 a)
{
    return ((pixel_t)a << 24) | ((pixel_t)b << 16) | ((pixel_t)g << 8) | (pixel_t)r;
}

extern void fb_init(void);
extern void fb_set_pixel(unsigned int x, unsigned int y, pixel_t pixel);

#endif // DRIVERS_FB_H
