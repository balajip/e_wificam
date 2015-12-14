/*
 * Copyright (C) 2010  VN Inc.
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
#ifndef __IRRC_H__
#define __IRRC_H__
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif


#define MOZART_IRRC_MAJOR		0		
#define DEVICE_NAME				"irrc"

#define IRRC_BASE				MOZART_IRDAC_MMR_BASE
#define V_IRRC_BASE     			IO_ADDRESS(IRRC_BASE)

#define MOZART_IRRC_READL(reg)				__raw_readl(V_IRRC_BASE + (reg))
#define MOZART_IRRC_WRITEL(reg,val)			__raw_writel((val), V_IRRC_BASE + (reg))


#define IRDA_FREQ				APB_CLK
//#define IRDA_FREQ				100000000
#define IR_SAMPLING_TIME		10		//unit:micro-second
#define IR_FREQ_DIV				(IR_SAMPLING_TIME*IRDA_FREQ)/1000000

#define IRRC_VERSION			0x00
#define IRRC_CTRL				0x04
#define IRRC_STAT				0x08
#define IRRC_PBR_LEFT			0x0C
#define IRRC_PBR_RIGHT			0x10
#define IRRC_TIMING				0x14
#define IRRC_TIMING_EXT			0x18
#define IRRC_INTERVAL_FREQ_DIV	0x1C

#define DATA_NUM_BIT			16
#define DRIFT_BIT				10
#define MODU_TYPE_BIT			8
#define INV_SDA_EN_BIT			7
#define IRRX_ENDIAN_BIT			6
#define BURST_MODE_EN_BIT		5
#define FIFO_EN_BIT				4
#define RST_EN_BIT				3
#define OP_START_BIT			2
#define OP_CMPT_ACK_EN_BIT		1
#define OP_CMPT_ACK_BIT		0

#define DATA_NUM_MASK			0x1F<<DATA_NUM_BIT
#define DRIFT_MASK				0x3<<DRIFT_BIT
#define MODU_TYPE_MASK		0x3<<MODU_TYPE_BIT

#define REPEAT_BIT				3
#define FIFO_FULL_BIT			2
#define FIFO_NOT_EMPTY_BIT		1
#define OP_CMPT_BIT		0

#define MODU_LEN_BIT			16
#define ZERO_LEN_BIT			8
#define ONE_LEN_BIT				0

#define MODU_LEN_MASK			0xFF<<MODU_LEN_BIT
#define ZERO_LEN_MASK			0xFF<<ZERO_LEN_BIT
#define ONE_LEN_MASK			0xFF<<ONE_LEN_BIT

#define DEBOUNCE_BIT			24
#define REPEAT_LEN_BIT			16
#define SILEN_LEN_BIT			8
#define BURST_LEN_BIT			0

#define DEBOUNCE_MASK			0xFF<<DEBOUNCE_BIT
#define REPEAT_LEN_MASK		0xFF<<REPEAT_LEN_BIT
#define SILEN_LEN_MASK			0xFF<<SILEN_LEN_BIT
#define BURST_LEN_MASK			0xFF<<BURST_LEN_BIT

#define REPEAT_INTERVAL_BIT	0
#define FREQ_DIV_BIT			16

#define REPEAT_INTERVAL_MASK	0xFFFF
#define FREQ_DIV_MASK			0xFFFF0000

typedef enum {
	NEC = 0,
	RC5 = 1,
	Sharp = 2,
	SonySIRC = 3,
	NEC64 = 4,
} IrrcProtocols;

typedef enum {
	IRRC_50_PERCENT = 0,
	IRRC_25_PERCENT = 1,
	IRRC_12_5_PERCENT = 2,
} IrrcCTRL_Drift;

typedef enum {
	IRRC_BI_PHASE = 0,
	IRRC_PULSE_DIST = 1,
	IRRC_PULSE_WIDTH = 2,
} IrrcCTRL_ModuType;

typedef enum {
	IRRC_LSB = 0,
	IRRC_MSB = 1,
} IrrcCTRL_Endian;

typedef struct {
	int IRDN;			//DATA_NUM
	int IRDR;			//DRIFT
	int IRMT;			//MODU_TYPE 	
	int IRISE;		//INV_SDA_EN
	int IRENDIAN;	//IRRX_ENDIAN
	int IRBME;		//BURST_MODE_EN
	int IRFIE;		//FIFO_EN

	int IRRML;		//MODU_LEN
	int IRRDZL;		//ZERO_LEN
	int IRRDOL;		//ONE_LEN

	int IRRDL;		//DEBOUNCE
	int IRRRL;		//REPEAT_LEN
	int IRRSL;		//SILEN_LEN
	int IRRBL;		//BURST_LEN
	
	int IRFD;			//FREQ_DIV
	int IRRI;			//REPEAT_INTERVAL
} IrrcSettings;

#define IRRC_BUF_SIZE			8

#define RC5_TOGGLE_BIT			(11 + 32 - 14)

#define IRRC_IOC_MAGIC			0xe0
#define IRRC_RESET				_IO (IRRC_IOC_MAGIC, 0)
#define IRRC_SETPROTOCOL		_IOW(IRRC_IOC_MAGIC, 1, int)
#define IRRC_SETPARAM			_IOW (IRRC_IOC_MAGIC, 2, int)
#define IRRC_GETPARAM			_IOR (IRRC_IOC_MAGIC, 3, int)

#define IRRC_DRIVER_VERSION		"1.1.0.2"
#define IRRC_MAINTAINER			"Evelyn, Tsai"
#define IRRC_COMPANY				"Vtcs Inc."
#define IRRC_PLATFORM			"Mozart EVM"
