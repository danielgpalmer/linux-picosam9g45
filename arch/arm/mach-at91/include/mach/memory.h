/*
 * arch/arm/mach-at91/include/mach/memory.h
 *
 *  Copyright (C) 2004 SAN People
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#include <mach/hardware.h>

#ifdef CONFIG_MACH_PICOSAM9G45
#ifdef CONFIG_SPARSEMEM

/*
 * Sparsemem definitions for pico-SAM9G45
 *
 * The old patch for this had the high block mapped below the low block;
 * Code in other parts (mm.c if I remember correctly) do not like this..
 * The mapping below only works if the kernel is loaded into the lower 
 * bank. Check Makefile.boot for how this is achieved.
 *
 * 128MB @ 0x20000000 -> PAGE_OFFSET
 * 128MB @ 0x70000000 -> PAGE_OFFSET + 0x8000000
 */

#define BANK1 UL(0x20000000)
#define BANK2 UL(0x70000000)
#define BANKSIZE UL(0x8000000)
#define PLAT_PHYS_OFFSET      BANK1
#define MAX_PHYSMEM_BITS      32
#define SECTION_SIZE_BITS     27 /*128 Mb */

/* bank page offsets */
#define PAGE_OFFSET_HIGHBANK  (PAGE_OFFSET + BANKSIZE)

#define __phys_to_virt(p)   \
            (((p) & 0x07ffffff) + (((p) & 0x40000000) ? PAGE_OFFSET_HIGHBANK : PAGE_OFFSET))

#define __virt_to_phys(v)   \
            (((v) & 0x07ffffff) + (((v) & 0x08000000) ? BANK2 : BANK1 ))

#else
	#error The pico-sam9g45 requires sparsemem
#endif        /* CONFIG_SPARSEMEM */

#endif /* CONFIG_MACH_PICOSAM9G45 */

#endif
