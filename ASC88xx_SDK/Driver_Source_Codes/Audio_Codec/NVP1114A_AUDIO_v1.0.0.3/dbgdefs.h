/*
 * Mozart_Audio_NVP1114A
 * Driver for Mozart NVP1114A Audio Codec Driver
 *
 * Copyright (C) 2010  ______ Inc.
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

/* ======================================================================== */
#ifndef __DBGDEFS_H__
#define __DBGDEFS_H__

/* ======================================================================== */
#undef PDEBUG
#ifdef VAUDIO_DEBUG /* replace DRIVER with your program name */
#  ifdef __KERNEL__
//#    define LOGLEVEL	KERN_DEBUG
#    define LOGLEVEL	KERN_EMERG
#    define PDEBUG(fmt, args...) printk( LOGLEVEL "vaudio: " fmt, ## args)
#  else
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* do nothing */
#endif


#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* remove a debug printing command by add an ending G */

/* ======================================================================== */
#endif //__DBGDEFS_H__
