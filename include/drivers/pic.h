/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef DRIVERS_PIC_H
#define DRIVERS_PIC_H

#define PIC1_CMD  0x20
#define PIC2_CMD  0xa0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1

extern void pic_init(void);
extern void pic_eoi(int irq);

#endif // DRIVERS_PIC_H
