/*
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 *  bugficks@samygo 
 *      Copied and modified required stuff from various Samsung sources
 *
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TSD_INTERNAL_H__
#define __TSD_INTERNAL_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/bitmap.h>
#include <linux/of.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//14xx
#define MUX_SEL_4			0x005C0108
#define MUX_SEL_5			0x005C010C
#define MUX_SEL_6			0x005C0110

//1501
#define	MUX_SEL_0			0x005C066C
#define	MUX_SEL_1			0x005C0670
#define	P0_DLY_SEL_0		0x005C0690
#define	P0_DLY_SEL_1		0x005C0694
#define	P1_DLY_SEL_0		0x005C0698
#define	P1_DLY_SEL_1		0x005C069C
#define	P2_DLY_SEL_0		0x005C06A0
#define	P2_DLY_SEL_1		0x005C06A4
#define	P3_DLY_SEL_0		0x005C06A8
#define	P3_DLY_SEL_1		0x005C06AC
#define	P1_CICAM_DLY_SEL_0	0x005C06B0
#define	P1_CICAM_DLY_SEL_1	0x005C06B4
#define	P3_CICAM_DLY_SEL_0	0x005C06B8
#define	P3_CICAM_DLY_SEL_1	0x005C06BC

#define TSD_DMEM_BASE		(0x8000)

#define SW_REG_BASE			(0x86c0)
#define SW_REG_SIZE			(0xd40)

#define SF_MEM_BASE			(0xc000)
#define SF_MEM_SIZE			(0x2000)

/* TSD S/W registers */
#define SW_PCONT(n)			(0x86c0 + (n) * 4) /* 0 ~ 191 */
#define SW_Q_START(n)		(0x89c0 + (n) * 4) /* 0 ~ 191 */
#define SW_Q_PKT(n)			(0x8cc0 + (n) * 4) /* 0 ~ 191 */
#define SW_Q_SEC(n)			(0x8fc0 + (n) * 4) /* 0 ~ 191 */

#define SW_BANK_CTRL(n)		(0x92c0 + (n) * 4) /* 0 ~ 5 */
#define SW_RISC_INT			(0x92d8)
#define SW_PCR_INT_MASK		(0x92dc)
#define SW_SCRM_CTRL		(0x9310)
#define SW_PROT_CTRL		(0x9310)

#define SW_SF_BASE			(0xC000)

/* i: 0 ~ 3, n: 0 ~ 95, m: 0 ~ 1 */  //CHECK MR WOOSUNG Expanding for two Section Filters
#define SW_SF_DATA(i, n, m)	(0xc000 + (n) * 0x10 + (i) * 0x600 + (m) * 0x2000)
#define SW_SF_MASK(i, n, m)	(0xc004 + (n) * 0x10 + (i) * 0x600 + (m) * 0x2000)
#define SW_SF_MODE(i, n, m)	(0xc008 + (n) * 0x10 + (i) * 0x600 + (m) * 0x2000)

/* i: 0 ~ 2 (0: 0 ~ 31, 1: 32 ~ 63, 2: 64 ~ 95), m: 0~1 */
#define SW_SF_SEL(i, n, m)	(0xd800 + (n) * 0x10 + (i) * 4 + (m) * 0x2000) /* n: 0 ~ 95 */

/* PCR error count register */
#define SW_TSD_ERR_PCR_ABNORMAL_WRAP(n) 			(0xd10c + (n)*0x10)
#define SW_TSD_ERR_PCR_DISCONTINUUITY(n)			(0xd13c + (n)*0x10)
#define SW_TSD_ERR_PCR_ABNORMAL_BITRATE(n)			(0xd16c + (n)*0x10)
#define SW_PCR_DISCON_DIFF_BASIS_IN_ISR(n)			(0xd19c + (n)*0x10)
#define SW_PCR_DISCON_DIFF_BASIS_IN_ONESHOT(n)		(0xd1cc + (n)*0x10)
#define SW_STC_PCR_ABNORMAL_BITRATE_DIFF_BASIS(n)	(0xd20c + (n)*0x10)
#define SW_TSD_PCR_VALUE(n)							(0xd22c + (n)*0x10)
#define SW_TSD_PRE_PCR_VALUE(n)						(0xd25c + (n)*0x10)
#define SW_TSD_BUGFIX_PRE_PCR(n)					(0xd28c + (n)*0x10)
#define SW_TSD_PRE_PCR_DIFF(n)						(0xd2bc + (n)*0x10)
#define SW_TSD_STC_LATCHED_VALUE(n)                 (0xd2ec + (n)*0x10)
#define SW_TSD_LATCHED_STC_PCR_DIFF(n)              (0xd31c + (n)*0x10)
#define SW_PCR_TARGET_EXT_MODULE(n)					(0xd34c + (n)*0x10)
#define SW_ONSHOT_PKT_CNT_DIFF_VALUE(n)				(0xd37c + (n)*0x10)
#define SW_ABS_PCR_DIFF_VALUE(n)					(0xd3ac + (n)*0x10)
#define SW_ONSHOT_CUR_STC_VALUE(n)					(0xd3dc + (n)*0x10)
#define SW_ONSHOT_PRE_STC_VALUE(n)					(0xd40c + (n)*0x10)
#define SW_ONSHOT_PCR_BITRATE_BANKNUM(n)			(0xd43c + (n)*0x10)
#define SW_ONESHOT_PCR_BITRATE_INFORM_PERIOD(n)		(0xd46c + (n)*0x10)

#define SW_DEC_CLK_ENABLE_STS						(0xd49c)
#define SW_DEBUG_INTERRUPT_PRINT					(0xd4ac)
#define SW_DEBUG_SECTION_MONITOR_PID				(0xd4bc)
#define SW_REG_TSD_DEBUG_CA_OVERFLOW_COUNT			(0xd4cc)

/* Dual decoding */
/* n 0 ~ 1 */
#define SW_SECOND_TARGET_CTRL(n)	(0xd54c + (n) * 0x10) /* 0 ~ 1 */
#define SW_SECOND_Q_START(n)		(0xd57c + (n) * 0x10) /* 0 ~ 1 */
#define SW_SECOND_Q_WRITE(n)		(0xd5ac + (n) * 0x10) /* 0 ~ 1 */

#define SW_INIT_K_VALUE				(0xd61c)
#define SW_HCAS_DELAY				(0xd62c)
#define SW_TSD_PCR_RECOVERY_SET(n)	(0xd63c + (n) * 0x10)
#define SW_TSD_DEBUG_PCR_VAL(n)		(0xd65c + (n) * 0x10)
#define SW_TSD_DEBUG_DIFF_STC_PCR(n)	(0xd67c + (n) * 0x10)
#define SW_TSD_WAIT_PWM_CHANGE(n)	(0xd69c + (n) * 0x10)

#define TSD_REG0_ADDR		(0x10070)
#define TSD_REG0_SIZE		(0x290)

#define TSD_REG1_ADDR		(0x10600)
#define TSD_REG1_SIZE		(0x14C)

#define TSD_REG2_ADDR		(SW_REG_BASE)
#define TSD_REG2_SIZE		(SW_REG_SIZE)

#define TSD_REG3_ADDR		(SW_SF_BASE)
#define TSD_REG3_SIZE		(0x4000)

#define TSD_REG4_ADDR		(0x10040)
#define TSD_REG4_SIZE		(0x2C)

#define TSD_REG5_ADDR(n)	(TSD_RISC_QUEUE_MASK(n))
#define TSD_REG5_SIZE		0x18

#define TSD_REG6_ADDR		(TSD_RISC_INT_MASK)
#define TSD_REG6_SIZE		0x4

#define TSD_REG7_ADDR		(0x10500)
#define TSD_REG7_SIZE		0x5EC

//14xx
#define TSD_TOP_REG0_ADDR	(0x5C0108)
#define TSD_TOP_REG0_SIZE	(0xC)

#define TSD_TOP_REG1_ADDR	(0x5c0140)
#define TSD_TOP_REG1_SIZE	(0x4)

#define TSD_TOP_REG2_ADDR	(0x5c0150)
#define TSD_TOP_REG2_SIZE	(0x4)

#define TSD_TOP_REG3_ADDR	(0x5c00c0)
#define TSD_TOP_REG3_SIZE	(0x4)

#define TSD_TOP_REG4_ADDR	(0x5c00d0)
#define TSD_TOP_REG4_SIZE	(0x4)

#define TSD_TOP_REG5_ADDR	(0x6c)
#define TSD_TOP_REG5_SIZE	(0x4)

//1501
//#define TSD_TOP_REG0_ADDR	(0x5c066c)
//#define TSD_TOP_REG0_SIZE	(0x10)
//
//#define TSD_TOP_REG1_ADDR	(0x5c0690)
//#define TSD_TOP_REG1_SIZE	(0x30)
//
//#define TSD_TOP_REG2_ADDR	(0x5c0638)
//#define TSD_TOP_REG2_SIZE	(0x20)
//
//#define TSD_TOP_REG3_ADDR	(0x5c0510)
//#define TSD_TOP_REG3_SIZE	(0x4)
//
//#define TSD_TOP_REG4_ADDR	(0x5c083c)
//#define TSD_TOP_REG4_SIZE	(0x4)
//
//#define TSD_TOP_REG5_ADDR	(0x94)
//#define TSD_TOP_REG5_SIZE	(0x4)

/* TSD H/W registers */
#define TSD_SW_RST				0x10000
#define TSD_INTR_IESR			0x10004
#define TSD_INTR_IPSR			0x10008
#define TSD_INTR_IESR2			0x10014
#define TSD_INTR_IPSR2			0x10018
#define TSD_INTR_IESR3			0x10c00
#define TSD_INTR_IPSR3			0x10c04
#define TSD_RISC_PM_SEL			0x10030
#define TSD_EXT_IF_CTRL1		0x10040
#define TSD_EXT_IF_CTRL1_2		0x10044
#define TSD_EXT_IF_CTRL3		0x10050
#define TSD_EXT_IF_CTRL3_2		0x10054
#define TSD_EXT_IF_CTRL4		0x10058
#define TSD_EXT_IF_CTRL5		0x1005c
#define TSD_EXT_IF_CTRL6		0x10060
#define TSD_M_EXT_IF_CTRL5		0x10064
#define TSD_PID_IN_SEL			0x10070
#define TSD_PID_IN_SEL2			0x1006c
#define TSD_EXT_IF_STATUS		0x10074
#define TSD_EXT_IF_STATUS_2		0x10078
#define TSD_EXT_IF_CTRL4_2		0x10f74
#define TSD_M_EXT_IF_CTRL5_2	0x10f78
#define TSD_M_EXT_IF_CTRL6		0x10f7c

#define TSD_STC_BASE(n)			(0x10500 + (n) * 0xc)	/* 500, 50c, 518, 524, 530, 53c, 548 */
#define TSD_STC_EXT(n)			(0x10504 + (n) * 0xc)	/* 504, 510, 51c, 528, 534, 540, 54c */
#define TSD_STC_CTRL(n)			(0x10508 + (n) * 0xc)	/* 508, 514, 520, 52c, 538, 544, 550 */
#define TSD_PKT_PCR_BASE(n)		(0x10568 + (n) * 0x10)
#define TSD_PCR_REG_IDX			(0x10590)
#define TSD_PWM_CTRL(n)			(0x10594 + (n) * 0x4)

#define TSD_27M_TIMER_INT_EN	(0x105f0)
#define TSD_27M_TIMER_INT_CNT	(0x105f4)
#define TSD_27M_TIMER_INT_STS	(0x105f8)

#define TSD_PID(n)				(0x10100 + (n) * 4)		/* 0 ~ 191*/

#define TSD_RISC_PC				(0x10010)
#define TSD_TS_BUF_VAILD		(0x100e0)
#define TSD_CA_BUF_VAILD		(0x100f4)

/* BELOW ALL NEED TO CHANGE */
#define TSD_PVR_RDMA_CTRL(n)	(0x11600 + (n) * 0x20)	/* 1600, 1620, 1640, 1660 */
#define TSD_PVR_RDMA_WRPTR(n)	(0x11604 + (n) * 0x20)	/* 1604, 1624, 1644, 1664 */
#define TSD_PVR_RDMA_SA0(n)		(0x11608 + (n) * 0x20)	/* 1608, 1628, 1648, 1668 */
#define TSD_PVR_RDMA_SA1(n)		(0x1160c + (n) * 0x20)	/* 160c, 162c, 164c, 166c */
#define TSD_PVR_RDMA_DPC(n)		(0x11610 + (n) * 0x20)	/* 1610, 1630, 1650, 1670 */
#define TSD_PVR_RDMA_QSIZE(n)	(0x11614 + (n) * 0x20)	/* 1614, 1634, 1654, 1674 */
#define TSD_PVR_RDMA_RDPTR(n)	(0x1161c + (n) * 0x20)	/* 161c, 163c, 165c, 167c */
#define TSD_PVR_WDMA_CTRL		(0x11400)
#define TSD_PVR_WDMA_STS		(0x11404)
#define TSD_PVR_WDMA_TA0(n)		(0x11410 + (n) * 0x20)	/* 1410, 1430, 1450, 1470 */
#define TSD_PVR_WDMA_TA1(n)		(0x11414 + (n) * 0x20)	/* 1414, 1434, 1454, 1474 */
#define TSD_PVR_WDMA_DPC(n)		(0x11418 + (n) * 0x20)	/* 1418, 1438, 1458, 1478 */
#define TSD_PVR_WDMA_QSIZE(n)	(0x1141c + (n) * 0x20)	/* 141c, 143c, 145c, 147c */
#define TSD_PVR_WDMA_PCNT(n)	(0x11420 + (n) * 0x20)	/* 1420, 1440, 1460, 1480 */
#define TSD_PVR_RD_CTRL(n)		(0x11500 + (n) * 0x20)	/* 1500, 1520, 1540, 1560 */
#define TSD_PVR_RD_IF_STATUS(n)	(0x11514 + (n) * 0x20)	/* 1514, 1534, 1554, 1574 */

#define TSD_PVR_WR_CTRL(n)		(0x11100 + (n) * 0x80)	/* 1100, 1180, 1200, 1280 */
#define TSD_PVR_WR_PTS_HIGH(n)	(0x11104 + (n) * 0x80)	/* 1104, 1184, 1204, 1284 */
#define TSD_PVR_WR_PTS_LOW(n)	(0x11108 + (n) * 0x80)	/* 1108, 1188, 1208, 1288 */
#define TSD_PVR_WR_HTYPE(n)		(0x11118 + (n) * 0x80)	/* 1118, 1198, 1218, 1298 */
#define TSD_PVR_WR_PES(n)		(0x1111c + (n) * 0x80)	/* 111c, 119c, 121c, 129c */
#define TSD_PVR_WR_SEQ(n)		(0x11120 + (n) * 0x80)	/* 1120, 11a0, 1220, 12a0 */
#define TSD_PVR_WR_PIC(n)		(0x11124 + (n) * 0x80)	/* 1124, 11a4, 1224, 12a4 */
#define TSD_PVR_WR_META_DATA(n)	(0x11134 + (n) * 0x80)	/* 1134, 11b4, 1234, 12b4 */
#define TSD_PVR_WR_RAP(n)		(0x11138 + (n) * 0x80)	/* 1138, 11b8, 1238, 12b8 */
#define TSD_PVR_WR_SPS(n)		(0x11144 + (n) * 0x80)	/* 1144, 11c4, 1244, 12c4 */
#define TSD_PVR_WR_SLICE(n)		(0x1114c + (n) * 0x80)	/* 114c, 11cc, 124c, 12cc */
#define TSD_PVR_WR_AVS_SEQ(n)	(0x11154 + (n) * 0x80)	/* 1154, 11d4, 1254, 12d4 */
#define TSD_PVR_WR_AVS_IPIC(n)	(0x11158 + (n) * 0x80)	/* 1158, 11d8, 1258, 12d8 */
#define TSD_PVR_WR_AVS_PBPIC(n)	(0x1115c + (n) * 0x80)	/* 115c, 11dc, 115c, 12dc */
#define TSD_PVR_WR_HEVC_SEQ(n)		(0x11168 + (n) * 0x80)
#define TSD_PVR_WR_HEVC_PIC(n)		(0x1116c + (n) * 0x80)
#define TSD_PVR_WR_HEVC_PIC_TYPE(n)	(0x11170 + (n) * 0x80)

#define TSD_HCAS_MUX_CTRL(n)		(0x11700 + (n) * 0x8)
#define TSD_HCAS_MUX_STREAM_ID(n)	(0x11704 + (n) * 0x8)
#define TSD_HCAS_DEMUX_CTRL			(0x11e00)
#define TSD_HCAS_DEMUX_STATUS		(0x11e04)
#define TSD_HCAS_DEMUX_SEL_CTRL(n)	(0x11e10 + (n) * 0x8)
#define TSD_HCAS_DEMUX_STREAM_ID(n)	(0x11e14 + (n) * 0x8)

#define TSD_CA_CTRL(n)			(0x10600 + (n) * 0x4)	/* 600, 604, 608 */

#define TSD_DS1_RISC_QUEUE_PEND(n)		(0x10750 + (n)*0xc)
#define	TSD_DS1_RISC_QUEUE_MASK(n)		(0x10754 + (n)*0xc)
#define TSD_DS1_RISC_QUEUE_CLR(n)		(0x10758 + (n)*0xc)

#define TSD_RISC_QUEUE_PEND(n)	(0x10c80 + (n) * 0xc)	/* c80, c8c, c98 */
#define TSD_RISC_QUEUE_MASK(n)	(0x10c84 + (n) * 0xc)	/* c84, c90, c9c */
#define TSD_RISC_QUEUE_CLR(n)	(0x10c88 + (n) * 0xc)	/* c88, c94, ca0 */
#define RISCTO_HOSTINT          (0x10cc8)
#define RISCTO_HOSTINT_CLR      (0x10ccc)
#define TSD_RISC_INT_MASK		(0x10cd0)
#define TSD_RISC_INT_PEND		(0x10cd4)
#define TSD_IN_INTR_EN			(0x10f20)
#define TSD_IN_INTR_EN_2		(0x10f24)

#define TSD_INPUT_WDMA_CTRL		(0x10b00)
#define TSD_INPUT_WDMA_TA0		(0x10b10)
#define TSD_INPUT_WDMA_TA1		(0x10b14)
#define TSD_INPUT_WDMA_DPC		(0x10b18)
#define TSD_INPUT_WDMA_QSIZE	(0x10b1c)
#define TSD_INPUT_WDMA_WR_ADDR	(0x10b24)
#define TSD_INPUT_WDMA_SYNC_CNT	(0x10b30)

/* TSD_SW_RST bits */
#define SW_RST_RISC					BIT(0)
#define SW_RST_CA					BIT(2)
#define SW_RST_BUF_DMA				BIT(3)
#define SW_RST_PVR_WDMA				BIT(4)
#define SW_RST_PVR_RDMA1			BIT(5)
#define SW_RST_PVR_RDMA2			BIT(6)
#define SW_RST_PVR_RDMA3			BIT(7)
#define SW_RST_PVR_RDMA4			BIT(8)
#define SW_RST_HSF					BIT(9)
#define SW_RST_PVR_RDMA5			BIT(10)
#define SW_RST_HCAS					BIT(11)
#define SW_RST_INPUT_WDMA			BIT(12)

/* SW_BANK_CTRL(n) bits */
#define BANK_CTRL_CLK_SRC_BIT_MAX		(0x3)
#define BANK_CTRL_PCR_RECOVERY_BIT_MAX	(0x3)

#define BANK_CTRL_EN_CHK_CC				BIT(0)
#define BANK_CTRL_EN_RELOAD_STC			BIT(1)
#define BANK_CTRL_CLK_SRC(v)			(((v) & BANK_CTRL_CLK_SRC_BIT_MAX) << 2)
#define BANK_CTRL_CLK_SRC_TYPE			BIT(4)
#define BANK_CTRL_PCR_RECOVERY(v)		(((v) & BANK_CTRL_PCR_RECOVERY_BIT_MAX) << 5)
#define BANK_CTRL_CONN_VIDEO0			BIT(8)
#define BANK_CTRL_CONN_VIDEO1			BIT(9)	/* MFC0 */
#define BANK_CTRL_CONN_VIDEO2			BIT(10) /* MFC1 */
#define BANK_CTRL_CONN_VIDEO3			BIT(11)
#define BANK_CTRL_CONN_AUDIO0			BIT(12)
#define BANK_CTRL_CONN_AUDIO1			BIT(13)
#define BANK_CTRL_CONN_AUDIOAD			BIT(14)
#define BANK_CTRL_CONN_AUDIOAUX			BIT(15)
#define BANK_CTRL_CONN_VIDEO4			BIT(16) /* HEVC UHD */
#define BANK_CTRL_CONN_VIDEO5			BIT(17) /* H264 UHD */

/* TSD_STC_EXT(n) bits */
#define STC_EXT_STC_EXT(v)				((v) & 0x1ff)
#define STC_EXT_STC_BASE_LSB			BIT(10)

/* TSD_STC_CTRL(n) bits */
#define STC_CTRL_PID_INDEX_BIT_MAX		(0xff)
#define STC_CTRL_STC_27CLK_SEL_BIT_MAX	(0x3)

#define STC_CTRL_PID_INDEX(v)			((v) & STC_CTRL_PID_INDEX_BIT_MAX)
#define STC_CTRL_LOAD_PCR				BIT(8)
#define STC_CTRL_RELOAD_STC				BIT(9)
#define STC_CTRL_STC_SPEED				BIT(10)
#define STC_CTRL_STC_27CLK_SEL(v)		(((v) & STC_CTRL_STC_27CLK_SEL_BIT_MAX) << 11)

/* TSD_PCR_REG_IDX bits */
#define STC_NUM_BIT_MAX				(0x7)

#define STC_NUM_VIDEO0_BIT_PLACE	(18)
#define STC_NUM_VIDEO1_BIT_PLACE	(21)
#define STC_NUM_VIDEO2_BIT_PLACE	(24)

#define STC_NUM1(v)					(((v) & STC_NUM_BIT_MAX) << 0)
#define STC_NUM2(v)					(((v) & STC_NUM_BIT_MAX) << 3)
#define STC_NUM3(v)					(((v) & STC_NUM_BIT_MAX) << 6)
#define STC_NUM_AUDIO0(v)			(((v) & STC_NUM_BIT_MAX) << 9)
#define STC_NUM_AUDIO1(v)			(((v) & STC_NUM_BIT_MAX) << 12)
#define STC_NUM_AUDIO2(v)			(((v) & STC_NUM_BIT_MAX) << 15)
#define STC_NUM_VIDEO0(v)			(((v) & STC_NUM_BIT_MAX) << STC_NUM_VIDEO0_BIT_PLACE)
#define STC_NUM_VIDEO1(v)			(((v) & STC_NUM_BIT_MAX) << STC_NUM_VIDEO1_BIT_PLACE)
#define STC_NUM_VIDEO2(v)			(((v) & STC_NUM_BIT_MAX) << STC_NUM_VIDEO2_BIT_PLACE)

/* n: 0 ~ 3*/
#define BANK_FOR_PCR(v, n)			(((v) >> (n*3)) & STC_NUM_BIT_MAX)

/* TSD_EXT_IF_CTRLn bits */
#define IF_STREAM_MODE			BIT(0) /* only for IF1 */
#define IF_SYNC_LOSS(v)			(((v) & 0x3) << 1)
#define IF_SYNC_LOCK(v)			(((v) & 0x7) << 3)
#define IF_TS_RULE_CHECK		BIT(6)
#define IF_RESET_SYNC			BIT(7)
#define IF_SYNC_EN				BIT(8)
#define IF_SYNC_INMODE			BIT(9)
#define IF3_SYNC_EN_DONE		BIT(9) /* only for IF3 */
#define IF_SYNC_EN_DONE			BIT(10)
#define IF_EXT_OUT_EN			BIT(10)	/* only for IF3 */

#define IF_RESET_SYNC_PKT_UNIT	BIT(11)
#define IF_SYNC_EN_PKT_UNIT		BIT(12)
#define IF_EXT_OUT_SEL(v)		(((v) & 0x1F) << 13) /* only for IF3 */
#define IF_UNIFORM_OUT_EN		BIT(18) /* only for IF3 */
#define IF_EXT_INPUT_SEL		BIT(20) /* only for IF3 */

/* TSD_PID_IN_SEL bits */
#define PID_IN_SEL_BIT_MAX		(0x1f)

#define PID_IN_SEL(n, v)		(((v) & PID_IN_SEL_BIT_MAX) << ((n)*5))
#define PID_27M_SEL(n, v)		(((v) & 0x1) << (3 + ((n) << 2)))
#define PID_ATS_CTRL0(n, v)		(((v) & 0x1) << (16 + ((n) << 1)))
#define PID_ATS_CTRL1(n, v)		(((v) & 0x1) << (17 + ((n) << 1)))

/* EXT_OUT_SEL values */
#define EXT_OUT_SEL_EXT1		0x1
#define EXT_OUT_SEL_EXT3		0x3
#define EXT_OUT_SEL_EXT4		0x4
#define EXT_OUT_SEL_EXT5		0x5
#define EXT_OUT_SEL_EXT6		0x6

#define EXT_OUT_SEL_EXT1_2		0x9
#define EXT_OUT_SEL_EXT3_2		0xA
#define EXT_OUT_SEL_EXT4_2		0xB
#define EXT_OUT_SEL_EXT5_2		0xC
#define EXT_OUT_SEL_EXT6_2		0xD

#define EXT_OUT_SEL_PVR1_RD		0x11
#define EXT_OUT_SEL_PVR2_RD		0x12
#define EXT_OUT_SEL_PVR3_RD		0x13
#define EXT_OUT_SEL_PVR4_RD		0x14


/* PID_IN_SEL values */
#define PID_IN_SEL_EXT1			0x1
#define PID_IN_SEL_EXT3			0x3	/* woosung: need for consider */
#define PID_IN_SEL_EXT4			0x4
#define PID_IN_SEL_EXT5			0x5
#define PID_IN_SEL_EXT6			0x6

#define PID_IN_SEL_EXT1_2		0x9
#define PID_IN_SEL_EXT3_2		0xA
#define PID_IN_SEL_EXT4_2		0xB
#define PID_IN_SEL_EXT5_2		0xC
#define PID_IN_SEL_EXT6_2		0xD
#define PID_IN_SEL_EXTM_5		0xE
#define PID_IN_SEL_EXTM_6		0xF

#define PID_IN_SEL_PVR1			0x11
#define PID_IN_SEL_PVR2			0x12
#define PID_IN_SEL_PVR3			0x13
#define PID_IN_SEL_PVR4			0x14

#define PID_IN_SEL_HCAS0		0x18
#define PID_IN_SEL_HCAS1		0x19

/* input status register */
#define PID_IN_SEL_EXT1_2_SYNC_LOCKED	BIT(15)
#define PID_IN_SEL_EXT3_2_SYNC_LOCKED	BIT(18)
#define PID_IN_SEL_EXT6_SYNC_LOCKED	BIT(27)



/* TSD_INTR_IESR, TSD_INTR_IPSR bits */
#define INTR_RISC				BIT(0)
#define INTR_QUEUE0				BIT(1)
#define INTR_QUEUE1				BIT(2)
#define INTR_QUEUE2				BIT(3)
#define INTR_PVR_W1_RAMDOM		BIT(5)
#define INTR_PVR_W1_PES			BIT(6)
#define INTR_PVR_W1_SEQ			BIT(7)
#define INTR_PVR_W1_PIC			BIT(8)
#define INTR_PVR_W2_RAMDOM		BIT(9)
#define INTR_PVR_W2_PES			BIT(10)
#define INTR_PVR_W2_SEQ			BIT(11)
#define INTR_PVR_W2_PIC			BIT(12)
#define INTR_TIMER_INT			BIT(14)
#define INTR_BUF_DMA			BIT(15)
#define INTR_PVR_WDMA1_QUEUE0	BIT(16)
#define INTR_PVR_WDMA1_QUEUE1	BIT(17)
#define INTR_PVR_WDMA2_QUEUE0	BIT(18)
#define INTR_PVR_WDMA2_QUEUE1	BIT(19)
#define INTR_PVR_RDMA1_QUEUE0	BIT(20)
#define INTR_PVR_RDMA1_QUEUE1	BIT(21)
#define INTR_PVR_RDMA2_QUEUE0	BIT(22)
#define INTR_PVR_RDMA2_QUEUE1	BIT(23)
#define INTR_PVR_R1_TS_SYNC_ERR	BIT(24)
#define INTR_PVR_R2_TS_SYNC_ERR	BIT(25)
#define INTR_TS_IN_SYNC_LOCK	BIT(26)
#define INTR_TS_IN_SYNC_LOSS	BIT(27)
#define INTR_CA_BUF_ERR			BIT(28)
#define INTR_TS_IN_ERR			BIT(29)
#define INTR_PCR1				BIT(30)
#define INTR_PCR2				BIT(31)

/* TSD_INTR_IESR2, TSD_INTR_IPSR2 bits */
#define INTR_PVR_W3_RAMDOM		BIT(4)
#define INTR_PVR_W3_PES			BIT(5)
#define INTR_PVR_W3_SEQ			BIT(6)
#define INTR_PVR_W3_PIC			BIT(7)
#define INTR_PVR_W4_RAMDOM		BIT(8)
#define INTR_PVR_W4_PES			BIT(9)
#define INTR_PVR_W4_SEQ			BIT(10)
#define INTR_PVR_W4_PIC			BIT(11)
#define INTR_PVR_WDMA3_QUEUE0	BIT(12)
#define INTR_PVR_WDMA3_QUEUE1	BIT(13)
#define INTR_PVR_WDMA4_QUEUE0	BIT(14)
#define INTR_PVR_WDMA4_QUEUE1	BIT(15)
#define INTR_PVR_RDMA3_QUEUE0	BIT(16)
#define INTR_PVR_RDMA3_QUEUE1	BIT(17)
#define INTR_PVR_RDMA4_QUEUE0	BIT(18)
#define INTR_PVR_RDMA4_QUEUE1	BIT(19)
#define INTR_PVR_R3_TS_SYNC_ERR	BIT(20)
#define INTR_PVR_R4_TS_SYNC_ERR	BIT(21)
#define INTR_QUEUE3				BIT(23)
#define INTR_QUEUE4				BIT(24)
#define INTR_QUEUE5				BIT(25)


#define INTR_DEFAULT (/*INTR_RISC |*/ INTR_QUEUE0 | INTR_QUEUE1 | INTR_QUEUE2 | \
        INTR_PVR_W1_SEQ | INTR_PVR_W1_PIC | \
        INTR_PVR_W2_SEQ | INTR_PVR_W2_PIC | \
        INTR_PVR_WDMA1_QUEUE0 | INTR_PVR_WDMA1_QUEUE1 | \
        INTR_PVR_WDMA2_QUEUE0 | INTR_PVR_WDMA2_QUEUE1 | \
        INTR_PVR_RDMA1_QUEUE0 | INTR_PVR_RDMA1_QUEUE1 | \
        INTR_PVR_RDMA2_QUEUE0 | INTR_PVR_RDMA2_QUEUE1 | \
        INTR_TS_IN_SYNC_LOCK | INTR_TS_IN_SYNC_LOSS | \
        INTR_CA_BUF_ERR | INTR_TS_IN_ERR)

#define INTR_DEFAULT2 ( \
        INTR_PVR_W3_SEQ | INTR_PVR_W3_PIC | \
        INTR_PVR_W4_SEQ | INTR_PVR_W4_PIC | \
        INTR_PVR_WDMA3_QUEUE0 | INTR_PVR_WDMA3_QUEUE1 | \
        INTR_PVR_WDMA4_QUEUE0 | INTR_PVR_WDMA4_QUEUE1 | \
        INTR_PVR_RDMA3_QUEUE0 | INTR_PVR_RDMA3_QUEUE1 | \
        INTR_PVR_RDMA4_QUEUE0 | INTR_PVR_RDMA4_QUEUE1 | \
        INTR_QUEUE3 | INTR_QUEUE4 | INTR_QUEUE5 \
        )

/* TSD_PID(n) bits */
#define PID_PID(v)				((v) & 0x1fff)
#define PID_PID_EN				BIT(13)
#define PID_PPE_IND				BIT(14)
#define PID_DMX_EN				BIT(15)
#define PID_EXTR_PAYLOAD		BIT(16)
#define PID_H264_OUT_EN			BIT(18)
#define PID_PVR_EN				BIT(19)
//14xx
#define PID_REPLACE_EN			BIT(20)
//15xx
#define PID_HCAS_EN				BIT(20)
#define PID_BYPASS_EN			BIT(21)
#define PID_CA_TYPE(v)			(((v) & 0xf) << 22)
#define PID_CA_TYPE_MASK		(0xf << 22)
#define PID_BANK_SEL(v)			(((v) & 0x7) << 26)
#define PID_BANK(v)				(((v) >> 26) & 0x7)
#define PID_DSC_EN				BIT(29)

/* PID_CA_TYPE(n) values */
#define CA_BYPASS				0x0
#define CA_DES_ECB				0x1
#define CA_3DES_CBC				0x2
#define CA_3DES_ECB				0x3
#define CA_AES_ECB				0x4
#define CA_AES_CBC				0x5
#define CA_MULTI2				0x6
#define CA_DVB_CSA				0x7
#define CA_AES_CTR				0x8

/* TSD_PVR_RDMA_CTRL(n) bits */
#define PRC_READY_CLEAR			BIT(0)
#define PRC_RING_BUF_MODE		BIT(1)
#define PRC_DMA					BIT(2)
#define PRC_Q0_READY			BIT(4)
#define PRC_Q1_READY			BIT(5)
#define PRC_64BIT_SWAP			BIT(6)
#define PRC_READY_CLEAR_WR		BIT(16)
#define PRC_RING_BUF_MODE_WR	BIT(17)
#define PRC_DMA_WR				BIT(18)
#define PRC_Q0_READY_WR			BIT(20)
#define PRC_Q1_READY_WR			BIT(21)
#define PRC_64BIT_SWAP_WR		BIT(22)

/* TSD_PVR_WDMA_CTRL bits */
#define PWC_S1_TYPE				BIT(0)
#define PWC_S1_DMA				BIT(1)
#define PWC_S1_PKT_CNT			BIT(2)
#define PWC_S2_TYPE				BIT(4)
#define PWC_S2_DMA				BIT(5)
#define PWC_S2_PKT_CNT			BIT(6)
#define PWC_S3_TYPE				BIT(8)
#define PWC_S3_DMA				BIT(9)
#define PWC_S3_PKT_CNT			BIT(10)
#define PWC_S4_TYPE				BIT(12)
#define PWC_S4_DMA				BIT(13)
#define PWC_S4_PKT_CNT			BIT(14)
#define PWC_BYTE_SWAP			BIT(16)
#define PWC_S5_DMA				BIT(21)
#define PWC_S5_PKT_CNT			BIT(22)

/* TSD_PVR_RD_CTRL(n) bits */
#define PRCn_STOP				BIT(0)
#define PRCn_START				BIT(1)
#define PRCn_TIME_STAMP			BIT(3)
#define PRCn_CA_BUF_EMPTY(v)	(((v) & 0x1f) << 25)

/* TSD_PVR_WR_CTRL(n) bits */
#define PWCn_PID_MASK				0x7
#define PWCn_PID(v)				((v) & 0x7) /* 0: no write, v: bank + 1 */
#define PWCn_TEI_EN			BIT(4)
#define PWCn_TIME_STAMP			BIT(10)
#define PWCn_VIDEO_CODEC(v)		(((v) & 0x7) << 12)
#define PWCn_PKT_CNT_CLR		BIT(15)
#define PWCn_H264_INTR_SEL		BIT(16)
#define PWCn_RAP_INTR_SEL		BIT(17)
#define PWCn_PID_DONE(v)		(((v) & 0x7) << 20)
#define PWCn_INDEX_CLEAR		BIT(23)

#define VIDEO_CODEC_TYPE(n)		((n >> 12) & 0x7)
#define PVR_CODEC_MPEG2			0x0
#define PVR_CODEC_H264			0x1
#define PVR_CODEC_AVS			0x2
#define PVR_CODEC_USER			0x3
#define PVR_CODEC_HEVC			0x4
#define PVR_CODEC_NONE			0x7

/* TSD_PVR_WR_HTYPE(n) bits */
#define PIC_HEADER_TYPE(n)		((n >> 2) & 0x3)
#define PVR_PICTURE_NON_REF		0
#define PVR_PICTURE_I			1
#define PVR_PICTURE_P			2
#define PVR_PICTURE_B			3

/* TSD_PVR_WR_META_DATA(n) bits */
#define PVR_IDR_SLICE(n)		((n >> 3) & 0x1)
#define PVR_NAL_REF_IDC(n)		((n >> 1) & 0x3)

/* TSD_HCAS_MUX_CTRL(n) */
#define HMCn_PID_SET_MASK		(0x1fff)
#define HMCn_PID_SET(v)		((v) & 0x1fff)
#define HMCn_BANK_SET_MASK		(0xf << 16)
#define HMCn_BANK_SET(v)		(((v) & 0xf) << 16)
#define HMCn_MUX_EN				BIT(20)

/* TSD_HCAS_DEMUX_CTRL */
#define HDCn_STOP				BIT(0)
#define HDCn_START				BIT(1)
#define HDCn_WAIT_CNT(v)		(((v) & 0xfff) << 4)
#define HDCn_CA_BUF_EMPTY(v)	(((v) & 0x1f) << 16)

/* TSD_HCAS_DEMUX_SEL_CTRL(n) */
#define HDSCn_DEMUX_SEL(v)		((v) & 0x1)
#define HDSCn_DEMUX_ENABLE		BIT(4)

#define CA_CTRL_OFFSET			(8)						/* 0~7:Bank matching, 8~95:Pid matching */

/* TSD_CA_CTRL(n) bits */
#define CA_KEY_IV_INDEX(v)		((v) & 0x7f)	/* bank0: 0x0, bank1: 0x3, bank2: 0x6 */
#define CA_PCKP_KEY_INDEX(v)	((v) & 0x1f)			/* key idx 0~31 */
#define CA_HCAS_SCR_EN			BIT(5)
#define CA_HCAS_IV_EN			BIT(6)
#define CA_TYPE(v)				(((v) & 0xf) << 7)
#define CA_TYPE_MASK			(0xf << 7)
#define CA_PID_INDEX(v)			(((v) & 0xff) << 11)	/* pid idx 0~191 */
#define CA_PID_MASK				(0xff << 11)
#define CA_MATCH_TYPE			BIT(27)	/* 0:pid filter match, 1:pid bank match */
#define CA_RESIDUE_TYPE			BIT(26) /* 0:bypass, 1:scte52(cbc),xor(ctr) */
#define CA_EN					BIT(19)
#define CA_BANK_SEL(v)			(((v) & 0x7) << 20)
#define CA_IV_SEL(v)			(((v) & 0x7) << 28)
#define CA_HCAS_EN				BIT(31)

/* CA_RESIDUE_TYPE(n) values */
#define RESIDUE_BYPASS			0x0
#define RESIDUE_SCTE52			0x1
#define RESIDUE_XOR				0x1

/* TSD_IN_INTR_EN bits */
#define IN_INTR_EXT1_SYNC_LOCK		BIT(0)
#define IN_INTR_EXT3_SYNC_LOCK		BIT(2)
#define IN_INTR_EXT4_SYNC_LOCK		BIT(3)
#define IN_INTR_EXT5_SYNC_LOCK		BIT(4)
#define IN_INTR_EXT1_SYNC_LOSS		BIT(5)
#define IN_INTR_EXT3_SYNC_LOSS		BIT(7)
#define IN_INTR_EXT4_SYNC_LOSS		BIT(8)
#define IN_INTR_EXT5_SYNC_LOSS		BIT(9)
#define IN_INTR_EXT1_INVALID		BIT(10)
#define IN_INTR_EXT3_INVALID		BIT(12)
#define IN_INTR_EXT4_INVALID		BIT(13)
#define IN_INTR_EXT5_INVALID		BIT(14)

#define IN_INTR_EXT1_2_SYNC_LOCK	BIT(15)
#define IN_INTR_EXT1_2_SYNC_LOSS	BIT(16)
#define IN_INTR_EXT1_2_INVALID		BIT(17)
#define IN_INTR_EXT3_2_SYNC_LOCK	BIT(18)
#define IN_INTR_EXT3_2_SYNC_LOSS	BIT(19)
#define IN_INTR_EXT3_2_INVALID		BIT(20)

#define M_INTR_EXT5_SYNC_LOCK		BIT(21)
#define M_INTR_EXT5_SYNC_LOSS		BIT(22)
#define M_INTR_EXT5_INVALID			BIT(23)
#define M_INTR_EXT6_SYNC_LOCK		BIT(24)
#define M_INTR_EXT6_SYNC_LOSS		BIT(25)
#define M_INTR_EXT6_INVALID			BIT(26)
#define IN_INTR_EXT6_SYNC_LOCK		BIT(27)
#define IN_INTR_EXT6_SYNC_LOSS		BIT(28)
#define IN_INTR_EXT6_INVALID		BIT(29)

/* SW_RISC_INT bits */
#define RINTR_STC_UNLOCKED					BIT(0)
#define RINTR_STC_LOCKED					BIT(1)
#define RINTR_PCR_DISCONTINUITY_INDICATOR	BIT(2)
#define RINTR_PCR_ABNORMAL_WRAP				BIT(3)
#define RINTR_PCR_DISCONTINUITY_IN_ISR		BIT(4)
#define RINTR_PCR_DISCONTINUITY_IN_ONESHOT	BIT(5)
#define RINTR_PVR_PCR_DISCONTINUITY			BIT(6)
#define RINTR_PCR_ABNORMAL_BITRATE			BIT(7)
#define RINTR_STC_UNLOCKED1					BIT(8)
#define RINTR_STC_LOCKED1					BIT(9)
#define RINTR_PCR_DISCONTINUITY_INDICATOR1	BIT(10)
#define RINTR_PCR_ABNORMAL_WRAP1			BIT(11)
#define RINTR_PCR_DISCONTINUITY_IN_ISR1		BIT(12)
#define RINTR_PCR_DISCONTINUITY_IN_ONESHOT1	BIT(13)
#define RINTR_PVR_PCR_DISCONTINUITY1		BIT(14)
#define RINTR_PCR_ABNORMAL_BITRATE1			BIT(15)
#define RINTR_PVR_PCR_BITRATE_INFORM		BIT(24)
#define RINTR_PVR_PCR_BITRATE_INFORM1		BIT(25)
#define RINTR_PWM_LIMIT_WARNING				BIT(28)  // 150423
#define RINTR_ALRAM_EXPIRED					BIT(29)
#define RINTR_ALRAM_CANCELED				BIT(30)
#define RINTR_MEMORY_FAULT					BIT(31)

#define RINTR_DEFAULT (RINTR_STC_UNLOCKED | RINTR_STC_LOCKED | \
        RINTR_PCR_DISCONTINUITY_INDICATOR | RINTR_PCR_ABNORMAL_WRAP | \
        RINTR_PCR_DISCONTINUITY_IN_ISR | \
        RINTR_PCR_DISCONTINUITY_IN_ONESHOT | \
        RINTR_PVR_PCR_DISCONTINUITY | RINTR_PCR_ABNORMAL_BITRATE | \
        RINTR_PCR_ABNORMAL_WRAP1 | RINTR_PCR_DISCONTINUITY_IN_ISR1 | \
        RINTR_PCR_DISCONTINUITY_IN_ONESHOT1 | \
        RINTR_PVR_PCR_DISCONTINUITY1 | RINTR_PCR_ABNORMAL_BITRATE1 | \
        RINTR_PVR_PCR_BITRATE_INFORM | RINTR_PVR_PCR_BITRATE_INFORM1)

/* SW_PCONT(n) bits */
#define PCONT_DEMUX_MODE(v)				((v) & 0xf)
#define PCONT_DEMUX_MODE_MASK			0xf
#define PCONT_IS_BLOCKING_DMA			BIT(4)
#define PCONT_PCR_ONE_SHOT				BIT(6)
#define PCONT_PCR_EN					BIT(7)
#define PCONT_LOAD_PCR					BIT(8)
#define PCONT_UPDATE_PCR				BIT(10)
#define PCONT_WAIT_FIRST_PUSI			BIT(11)
#define PCONT_QUEUE_SIZE(v)				(((v) & 0x1fff) << 12)
#define PCONT_QUEUE_SIZE_MASK			(0x1fff << 12)
#define PCONT_TARGET_AREA(v)			(((v) & 0xf) << 25)
#define PCONT_TARGET_AREA_MASK			(0xf << 25)
#define PCONT_CLEAR_P_STATE				BIT(29)
#define PCONT_SECOND_TARGET_AREA(v)		(((v) & 0x3) << 30)
#define PCONT_SECOND_TARGET_AREA_MASK	(0x3 << 30)

/* PCONT_DEMUX_MODE(n) value */
#define DMX_MODE_TP					0x0
#define DMX_MODE_PES				0x1
#define DMX_MODE_PAYLOAD			0x2
#define DMX_MODE_SECTION			0x3
/* need for fixing that same 1304 driver. */
#define DMX_MODE_NONE				0x4
#define DMX_MODE_DVB_PES			0xe

/* PCONT_QUEUE_SIZE(n) value: n = size / 4096 byte */
#define Q_SZ(v)						((v) >> 12)

/* PCONT_TARGET_AREA(n) value */
#define AREA_VIDEO0					0x0
#define AREA_VIDEO1					0x1
#define AREA_AUDIO0					0x2
#define AREA_AUDIO1					0x3
#define AREA_AUDIO2					0x4
#define AREA_SECTION				0x8
#define AREA_VIDEO2					0x9
#define AREA_VIDEO4					0xc
#define AREA_VIDEO5					0xd

/* PCONT_SECOND_TARGET_AREA(n) value */
#define UD_NONE						0x0
#define UD_HEVC						0x1
#define UD_H264						0x2

/* SW_DEC_CLK_ENABLE_STS bits */
#define DEC_CLK_AUDIO_EN			BIT(0)
#define DEC_CLK_VIDEO_EN			BIT(1)
#define DEC_CLK_HEVC_UHD_EN			BIT(2)
#define DEC_CLK_H264_UHD_EN			BIT(3)

#define CA_INIT_IDLE_ALL			0x44444444

#define TSD_20MSEC_ON_27MHZ_DOMAIN	540000
#define TSD_100MSEC_ON_27MHZ_DOMAIN	2700000
#define TSD_10SEC_ON_27MHZ_DOMAIN	270000000
#define TSD_100MSEC_ON_45KHZ_DOMAIN	4500
#define TSD_2SEC_ON_45KHZ_DOMAIN	90000
#define TSD_1MSEC_ON_27MHZ_DOMAIN	27000
#define TSD_1USEC_ON_27MHZ_DOMAIN	27

#define TSD_HCAS_WORK_TIMING		(4 * TSD_1MSEC_ON_27MHZ_DOMAIN)

#define PID_NULL					0x1fff

#define TSD_SEC_FILTER_MAX			96

#define TSD_SYNC_LOSS_CNT			3
#define TSD_SYNC_LOCK_CNT			3

#define PVR_PKT_SIZE				(188)
#define INPUT_WDMA_DPC_SIZE			(200)
#define PLAY_BUF_SIZE				(10 * 188 * 1024)
#define MFD_BUF_LOW					(512 * 1024)
#define PVR_R_BUF_SIZE				(PVR_PKT_SIZE * 1024)
#define PVR_W_BUF_SIZE				(PVR_PKT_SIZE * 16384)
#define INPUT_W_BUF_SIZE			(INPUT_WDMA_DPC_SIZE * 16384)		// 
#define INPUT_W_BUF_REMAIN			(0x240000)							// 0x100000000 - (INPUT_W_BUF_SIZE*2*0x28f)
#define SHM_AUDIO_SIZE				(0x40000)
#define SHM_VIDEO_SIZE				(0x100000)
#define HCAS_PKT_SIZE				(192)
#define HCAS_Q_BUF_SIZE				(HCAS_PKT_SIZE * 1024 * 2)

#define PES_MAX_SIZE				(64 * 1024)
#define PES_HEADER_LEN				(6)
#define PES_START_CODE				(0x000001)

/* PVR Buffer control related macros */
#define MFD_BUF_OVER_THRESHOLD		SZ_512K
#define MFD_BUF_UNDER_THRESHOLD		SZ_1M
#define MFD_BUF_UNDER_THRESHOLD_LOWSD	SZ_512K
#define SD_MAX_RES					(720 * 576) /* SD video resolution */
#define HD_MAX_RES					(1920 * 1024) /* HD video resolution */

#define AUDIO_BUF_OVER_THRESHOLD	SZ_64K
#define AUDIO_MAX_FRAME_SIZE		6176
#define AUDIO_BUF_SIZE				SZ_256K

/*
 * For the convenience of setting the register which is related to PID index.
 * Use the upper bits of the PID index as the register index.
 * And, use the lower bits of the PID index as a shift value of bit set.
 */
#define ID_GRP(idx) 				(((idx) >> 5) & 0x3)
#define ID_BIT(idx) 				(1 << ((idx) & 0x1f))

#define ID_GRP1(idx)				(((idx)/28))
#define ID_BIT1(idx)				(1 << ((idx)%28))

/* PVR play polling times (jiffies) */
#define DEFAULT_DELAY				2
/* PVR time based record polling time (jiffies) */
#define DEFAULT_RECORD_DELAY		25

/* PVR audio index generation polling times (jifies) */
#define DEFAULT_AUDIO_INDEX_TIMEOUT	25

/* Change to 33bit address */
#define TSD64ADDR(x)				(((x&(1<<31))?0x0:0x100000000)|(u64)x)


///////////////////////////////////////////////////////////////////////////
// RegDefine.h

/*---------------------------------------------------------------------*/
/*                 [ TSD ]                                             */
/*---------------------------------------------------------------------*/
//#define TSD_REG_BASE							(0x18110000)
#define TSD_REG_DEVICE_OFFSET					(0x02000000)
#define TSD_REG_BASE							(0x10000)
//#define TSD_REG_DEVICE_OFFSET					(0)

#define TSD_GR_BASE(device_num) 				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)))
#define TSD_INTR_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x004)
#define TSD_INOUT_SW_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x040)
#define TSD_BUF_IF_BASE(device_num) 			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x060)
#define TSD_CLK_RECOV_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x500)
#define TSD_PID_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x100)
#define TSD_BUF_DMA_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1000)
#define TSD_CA_CTRL_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x600)

#define TSD_CW_TBL0_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1800)

#define TSD_RISC_INT_BASE(device_num)			(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xc80)
#define TSD_HSF_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xfa0)

/* TSD Global Register */
#define REG_TSD_SW_RST(device_num)				((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))))

/* TSD Interrupt Register */
#define REG_TSD_INTR_IESR(device_num, n)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x4+(0x10*(n))))
#define REG_TSD_INTR_IPSR(device_num, n)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x08+(0x10*(n))))
#define REG_TSD_INTR_MMSR(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x0c))

#define REG_TSD_RISC_PC(device_num)				((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x10))

#define REG_TSD_CLK_FREQ_CHECK_CTRL(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x24))
#define REG_TSD_CLK_FREQ_CHECK_NUM(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x28))

#define REG_TSD_SE_SBOOTPIN						(TSD_REG_BASE+0x2c)
#define REG_TSD_SE_RAM_SEL						(TSD_REG_BASE+0x30)					// 0:ROM, 1:RAM
#define REG_TSD_SE_AUTHDONE						(TSD_REG_BASE+0x38)

/* TSD INOUT SW Register */
#define REG_TSD_EXT_IF_CTRL(device_num, ext_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x40 +4*(ext_num)))	/*ext_num = 0,1,2,4*/
#define REG_TSD_EXT_IF_STS(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x74))	/* n = 0,1 */
#define REG_TSD_EXT_IF_STS2(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x78))

#define REG_TSD_CMP_OUT_IN_CTRL(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x80))
#define REG_TSD_CMP_OUT_BUF_STS(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x84))
#define REG_TSD_CMP_OUT_FILTER(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x88))
#define REG_TSD_CMP_OUT_FILTER2(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x8c))

#define REG_TSD_PID_IN_SEL(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x70))

#define REG_TSD_MCARD_IF_CTRL(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF00))
#define REG_TSD_MCARD_OUT_LTSID(device_num) 	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF04))
#define REG_TSD_MCARD_OUT_LTSID2(device_num) 	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF08))
#define REG_TSD_MCARD_IF_STS(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF0C))

#define REG_TSD_IN_INTR_EN(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF20))
#define REG_TSD_IN_INTR_EN2(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF24))
#define REG_TSD_INVALID_SIG_SIZE(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF28))
#define REG_TSD_SYNC_LOSS_SIZE(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF2C))
#define REG_TSD_PCK_CNT(device_num,bank_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF30+0x4*(bank_num))) /* TSD_100816_0 */

#define REG_TSD_MAXLINEAR_IF_CTRL(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF70))
#define REG_TSD_MXL_EXT_IF_CTRL(device_num, n)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF74+0x4*(n)))
#define REG_TSD_MUX_IF_CTRL(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF80))
#define REG_TSD_MUX_OUT_SEL(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF84))
#define REG_TSD_MUX_LTSID(device_num, n)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF88+4*(n)))
#define REG_TSD_MUX_BUF_STS(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF90))
#define REG_TSD_MUX_STS(device_num)				((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF94))
#define REG_TSD_MUX_WAIT_CNT(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF98))

/* TSD BUF IF Register */
#define REG_TSD_BUFFER_VALID(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xE0))
#define REG_TSD_CA_BUF_STS(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF0))
#define REG_TSD_CA_BUF_VALID(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF4))
#define REG_TSD_PKT_END_WAIT_TIME(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xF8))

/* TSD PCR RECOV Register */
#define REG_TSD_STC_BASE(device_num,stc_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x500 + 0x0c*(stc_num))) /*stc_num = 0,1,2,3,4,5*/
#define REG_TSD_STC_EXT(device_num,stc_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x504 + 0x0c*(stc_num)))
#define REG_TSD_STC_CTRL(device_num,stc_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x508 + 0x0c*(stc_num)))
#define REG_TSD_LATCH_STC_BASE(device_num,index)((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x560 + 0x10*(index))) /* index = 0,1,2 */
#define REG_TSD_LATCH_STC_EXT(device_num,index)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x564 + 0x10*(index)))
#define REG_TSD_PKT_PCR_BASE(device_num,index) 	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x568 + 0x10*(index)))
#define REG_TSD_PKT_PCR_EXT(device_num,index)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x56C + 0x10*(index)))
#define REG_TSD_PCR_REG_INDEX(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x590 ))
#define REG_TSD_PWM_CTRL(device_num,index) 		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x594 + 0x04*(index)))
#define REG_TSD_STC_TIME_PATTERN0(device_num,stc_num)	(TSD_REG_BASE + ((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x5a0 + 0x8*(stc_num))	/*0-5*/
#define REG_TSD_STC_TIME_PATTERN1(device_num,stc_num)	(TSD_REG_BASE + ((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x5a4 + 0x8*(stc_num))	/*0-5*/
#define REG_TSD_STC_TIME_MSBPOS(device_num,n)	(TSD_REG_BASE + ((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x5e0 + 4*(n))
#define REG_TSD_STC_TIME_UPDATE(device_num)		(TSD_REG_BASE + ((TSD_REG_DEVICE_OFFSET)*(device_num)) + 0x5e8)

/* TSD PID1 Register */
#define REG_TSD_PID(device_num,f_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x100+0x04*(f_num))) /* f_num = 0~63 */

/* TSD BUF DMA Register */
#define REG_TSD_BUF_DMA_CTRL(device_num)		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1000))
#define REG_TSD_BUF_DMA_STS(device_num) 		((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1004))
#define REG_TSD_BUF_DMA_SA(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1008))
#define REG_TSD_BUF_DMA_TA(device_num)			((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x100C))
#define REG_TSD_BUF_DMA_TA_WR_DONE(device_num)	((TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1010))

/* TSD HW Section Filter */
#define REG_TSD_SEC_FIL_CTRL(device_num) 					(TSD_HSF_BASE((device_num)))

/* TSD CA CTRL, f_num=0~1 */
#define REG_TSD_CA_CTRL(device_num,f_num)					((TSD_CA_CTRL_BASE(device_num)+4*(f_num)))
/* CA_CTRL0~CA_CTRL8 will be used for bank-mode */
#define REG_TSD_CA_CTRL_BANKMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+12*(f_num)))

/* jyp_hawk */
//#define REG_TSD_CA_CTRL_SLOTMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+36+4*(f_num)))
#define REG_TSD_CA_CTRL_SLOTMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+72+4*(f_num)))

/* TSD CA Initial vectors */
//IV TDES
#define REG_TSD_IV_TDES_EVEN_H(device_num,Iv_Index)			((TSD_CA_CTRL_BASE((device_num))+0x2f8+0x20*(Iv_Index)))	//even
#define REG_TSD_IV_TDES_EVEN_L(device_num,Iv_Index)			((TSD_CA_CTRL_BASE((device_num))+0x2fc+0x20*(Iv_Index)))
#define REG_TSD_IV_TDES_ODD_H(device_num,Iv_Index) 			((TSD_CA_CTRL_BASE((device_num))+0x308+0x20*(Iv_Index)))	//odd
#define REG_TSD_IV_TDES_ODD_L(device_num,Iv_Index) 			((TSD_CA_CTRL_BASE((device_num))+0x30c+0x20*(Iv_Index)))
/* IV AES */
#define REG_TSD_IV_AES_EVEN_4TH_QUATER(device_num,Iv_Index)	((TSD_CA_CTRL_BASE((device_num))+0x2f0+0x20*(Iv_Index)))	//even
#define REG_TSD_IV_AES_EVEN_3RD_QUATER(device_num,Iv_Index)	((TSD_CA_CTRL_BASE((device_num))+0x2f4+0x20*(Iv_Index)))
#define REG_TSD_IV_AES_EVEN_2ND_QUATER(device_num,Iv_Index)	((TSD_CA_CTRL_BASE((device_num))+0x2f8+0x20*(Iv_Index)))
#define REG_TSD_IV_AES_EVEN_1ST_QUATER(device_num,Iv_Index)	((TSD_CA_CTRL_BASE((device_num))+0x2fc+0x20*(Iv_Index)))
#define REG_TSD_IV_AES_ODD_4TH_QUATER(device_num,Iv_Index) 	((TSD_CA_CTRL_BASE((device_num))+0x300+0x20*(Iv_Index)))	//odd
#define REG_TSD_IV_AES_ODD_3RD_QUATER(device_num,Iv_Index) 	((TSD_CA_CTRL_BASE((device_num))+0x304+0x20*(Iv_Index)))
#define REG_TSD_IV_AES_ODD_2ND_QUATER(device_num,Iv_Index) 	((TSD_CA_CTRL_BASE((device_num))+0x308+0x20*(Iv_Index)))
#define REG_TSD_IV_AES_ODD_1ST_QUATER(device_num,Iv_Index) 	((TSD_CA_CTRL_BASE((device_num))+0x30c+0x20*(Iv_Index)))
/* MULTI2 */
#define REG_TSD_IV_MULTI2_H(device_num) 					(TSD_CA_CTRL_BASE((device_num))+0x2c0)
#define REG_TSD_IV_MULTI2_L(device_num) 					(TSD_CA_CTRL_BASE((device_num))+0x2c4)

/*
 * CW table
 */
/* Multi2-system CW */
#define REG_TSD_SYS_KEY(device_num,key_index)				((TSD_CA_CTRL_BASE((device_num))+0x2e4-0x4*(key_index)))

/* DES and Multi2-data CW by bank-matching (Cw_Index : 0~2) */
#define REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_H(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_L(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x04+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_H(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x08+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_L(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x0c+0x30*(Cw_Index)))	//odd
/* DES and Multi2-data CW by slot-matching (Cw_Index : 0~8) */
#define REG_TSD_CW_KEY_DES_MULTI2_SLOT_MATCH_EVEN_H(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x120+0x10*(Cw_Index)))	//even
#define REG_TSD_CW_KEY_DES_MULTI2_SLOT_MATCH_EVEN_L(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x120+0x04+0x10*(Cw_Index)))	//even
#define REG_TSD_CW_KEY_DES_MULTI2_SLOT_MATCH_ODD_H(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x120+0x08+0x10*(Cw_Index)))	//odd
#define REG_TSD_CW_KEY_DES_MULTI2_SLOT_MATCH_ODD_L(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x120+0x0c+0x10*(Cw_Index)))	//odd
/* TDES CW */
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_A_H(device_num,Cw_Index) 			((TSD_CW_TBL0_BASE((device_num))+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_A_L(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x04+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_B_H(device_num,Cw_Index) 			((TSD_CW_TBL0_BASE((device_num))+0x08+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_B_L(device_num,Cw_Index) 			((TSD_CW_TBL0_BASE((device_num))+0x0c+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_C_H(device_num,Cw_Index) 			((TSD_CW_TBL0_BASE((device_num))+0x10+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_EVEN_C_L(device_num,Cw_Index) 			((TSD_CW_TBL0_BASE((device_num))+0x14+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_A_H(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x18+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_A_L(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x1c+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_B_H(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x20+0x30*(Cw_Index)))//odd
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_B_L(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x24+0x30*(Cw_Index)))//odd
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_C_H(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x28+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_TDES_ODD_C_L(device_num,Cw_Index)			((TSD_CW_TBL0_BASE((device_num))+0x2c+0x30*(Cw_Index)))	//odd
/* AES CW */
#define REG_TSD_CW_TBL0_KEY_AES_EVEN_4TH_QUATER(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_AES_EVEN_3RD_QUATER(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x04+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_AES_EVEN_2ND_QUATER(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x08+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_AES_EVEN_1ST_QUATER(device_num,Cw_Index)	((TSD_CW_TBL0_BASE((device_num))+0x0c+0x30*(Cw_Index)))	//even
#define REG_TSD_CW_TBL0_KEY_AES_ODD_4TH_QUATER(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x10+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_AES_ODD_3RD_QUATER(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x14+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_AES_ODD_2ND_QUATER(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x18+0x30*(Cw_Index)))	//odd
#define REG_TSD_CW_TBL0_KEY_AES_ODD_1ST_QUATER(device_num,Cw_Index)		((TSD_CW_TBL0_BASE((device_num))+0x1c+0x30*(Cw_Index)))	//odd

/* TSD CALM INT , f_num=0~1 => modified TSD_ARM_QUEUE filter offset(12), 2004.04.07 */
#define REG_TSD_ARM_QUEUE_PEND(device_num,f_num)	((TSD_RISC_INT_BASE((device_num))+12*(f_num)))
#define REG_TSD_ARM_QUEUE_MASK(device_num,f_num)	((TSD_RISC_INT_BASE((device_num))+0x4+12*(f_num)))
#define REG_TSD_ARM_QUEUE_CLR(device_num,f_num)		((TSD_RISC_INT_BASE((device_num))+0x8+12*(f_num)))
#define REG_TSD_RISC2HOST_INT(device_num)			((TSD_RISC_INT_BASE((device_num))+0x30))
#define REG_TSD_RISC2HOST_INT_CLR(device_num)		((TSD_RISC_INT_BASE((device_num))+0x34))
#define REG_TSD_RISC_INT_MASK(device_num)			((TSD_RISC_INT_BASE((device_num))+0x38))
#define REG_TSD_RISC_INT_PEND(device_num)			((TSD_RISC_INT_BASE((device_num))+0x3c))


/*--------------- [PVR] ---------------- */
#define TSD_PVR_RDMA1_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1600)
#define TSD_PVR_RDMA2_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1620)
#define TSD_PVR_WDMA_BASE(device_num)				(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1400)
#define TSD_PVR_RD1_IF_BASE(device_num)			  	(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1500)
#define TSD_PVR_WR1_IF_BASE(device_num)			  	(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1100)
#define TSD_CPE_BASE(device_num)					(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0xda0)
#define TSD_PSI_BUF_BASE(device_num)		    	(TSD_REG_BASE+((TSD_REG_DEVICE_OFFSET)*(device_num))+0x1000)

/* PVR RDMA Register : n == player instance number */
#define REG_TSD_PVR_RDMA_CTRL(device_num, n)		(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n)))
#define REG_TSD_PVR_RDMA_WRPTR(device_num, n) 		(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x04)
#define REG_TSD_PVR_RDMA_SA0(device_num, n)			(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x08)
#define REG_TSD_PVR_RDMA_SA1(device_num, n)			(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x0c)
#define REG_TSD_PVR_RDMA_DPC(device_num, n)			(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x10)
#define REG_TSD_PVR_RDMA_QSIZE(device_num, n) 		(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x14)
#define REG_TSD_PVR_RDMA_RQSIZE(device_num, n)		(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x18)
#define REG_TSD_PVR_RDMA_RDPTR(device_num, n) 		(TSD_PVR_RDMA1_BASE(device_num)+(0x20*(n))+0x1c)

/* PVR WDMA Register : n == recorder instance number */
#define REG_TSD_PVR_WDMA_CTRL(device_num)			(TSD_PVR_WDMA_BASE(device_num))
#define REG_TSD_PVR_WDMA_STS(device_num)			(TSD_PVR_WDMA_BASE(device_num)+0x04)
#define REG_TSD_PVR_WDMA_TA_0(device_num, n)		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x10)
#define REG_TSD_PVR_WDMA_TA_1(device_num, n)		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x14)
#define REG_TSD_PVR_WDMA_DPC(device_num, n)			(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x18)
#define REG_TSD_PVR_WDMA_QSIZE(device_num, n) 		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x1c)
#define REG_TSD_PVR_WDMA_PCNT(device_num, n)		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x20)
#define REG_TSD_PVR_WDMA_WR_ADDR(device_num, n)		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x24)
#define REG_TSD_PVR_WDMA_WR_DONE(device_num, n)		(TSD_PVR_WDMA_BASE(device_num)+(0x20*(n))+0x28)

/* PVR READ1 I/F : n == player instance number */
#define REG_TSD_PVR_RD_CTRL(device_num, n)			(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n)))
//#define REG_TSD_PVR_RD_ITS(device_num, n) 			(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x04)
//#define REG_TSD_PVR_RD_ATS(device_num, n) 			(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x08)
#define REG_TSD_PVR_RD_WAIT_CNT(device_num, n)		(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x0C)
//#define REG_TSD_PVR_RD_ATC(device_num, n) 			(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x10)
#define REG_TSD_PVR_RD_STATUS(device_num, n)		(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x14)
//#define REG_TSD_PVR_RD_TRICK(device_num, n)			(TSD_PVR_RD1_IF_BASE(device_num)+(0x20*(n))+0x18)

/* TSD PVR WRITE1 I/F : n == recorder instance number, m == recorder index parser number */
#define REG_TSD_PVR_WR_CTRL(device_num, n)			(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n)))
#define REG_TSD_PVR_WR_PTS_HIGH(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x04)
#define REG_TSD_PVR_WR_PTS_LOW(device_num, n, m) 	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x08)
#define REG_TSD_PVR_WR_PTS_CNT(device_num, n, m) 	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x0c)
#define REG_TSD_PVR_WR_ATS(device_num, n) 			(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+0x10)
#define REG_TSD_PVR_WR_PCNT(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x14)
#define REG_TSD_PVR_WR_HTYPE(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x18)
#define REG_TSD_PVR_WR_PES(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x1c)
#define REG_TSD_PVR_WR_SEQ(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x20)
#define REG_TSD_PVR_WR_PIC(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x24)
//#define REG_TSD_PVR_WR_ITS(device_num, n) 			(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+0x28)
//#define REG_TSD_PVR_WR_ATS_MAX_DIFF(device_num, n)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+0x2c)
//#define REG_TSD_PVR_WR_ATS_MIN_DIFF(device_num, n)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+0x30)
#define REG_TSD_PVR_WR_META_DATA(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x34)
#define REG_TSD_PVR_WR_RAP(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x38)
#define REG_TSD_PVR_WR_AU(device_num, n, m)			(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x3c)
#define REG_TSD_PVR_WR_SEI(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x40)
#define REG_TSD_PVR_WR_SPS(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x44)
#define REG_TSD_PVR_WR_PPS(device_num, n, m) 		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x48)
#define REG_TSD_PVR_WR_SLICE(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x4c)
#define REG_TSD_PVR_WR_HEADER(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x50)
#define REG_TSD_PVR_WR_AVS_SEQ(device_num, n, m) 	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x54)
#define REG_TSD_PVR_WR_AVS_IPIC(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x58)
#define REG_TSD_PVR_WR_AVS_PBPIC(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x5c)
#define REG_TSD_PVR_WR_OTHER1(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x60)
#define REG_TSD_PVR_WR_OTHER2(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x64)
#define REG_TSD_PVR_WR_HEVC_SEQ(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x68)
#define REG_TSD_PVR_WR_HEVC_PIC(device_num, n, m)	(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x6c)
#define REG_TSD_PVR_WR_HEVC_PIC_TYPE(device_num, n, m)		(TSD_PVR_WR1_IF_BASE(device_num)+(0x80*(n))+(0x200*(m))+0x70)

/* PVR_130329 : for sub video index */
#define REG_TSD_PVR_WR_SUB_VID_CTRL(device_num)		(TSD_PVR_WR1_IF_BASE(device_num)+0x200)

/* CPE CTRL : n == record instance number */
#define REG_TSD_CPE_CTRL(device_num, n) 		   	(TSD_CPE_BASE(device_num)+4*(n))
#define REG_TSD_CPE_KEY_0(device_num, n)			(TSD_CPE_BASE(device_num)+(0x10*(n))+0x08)
#define REG_TSD_CPE_KEY_1(device_num, n)			(TSD_CPE_BASE(device_num)+(0x10*(n))+0x0c)
#define REG_TSD_CPE_KEY_2(device_num, n)			(TSD_CPE_BASE(device_num)+(0x10*(n))+0x10)
#define REG_TSD_CPE_KEY_3(device_num, n)			(TSD_CPE_BASE(device_num)+(0x10*(n))+0x14)

typedef enum CaEvenOddType_t
{
  EVEN_KEY = 0x0,
  ODD_KEY = 0x1,
  NUM_OF_CWTYPE = 0x2,
}CaEvenOddType_t;

typedef enum
{
    CW_TABLE0 = 0,
    CW_TABLE1
} CwTable_t;

// CA_INIT_IDLE_ALL:
//    10001000      100        0       100       0       100       0        100      0         100      0       100
// * |31... 23| 22 21 20|     19 | 18 17 16|    15  |14 13 12 |     11 | 10  9  8|    7   | 6  5  4 |   3    | 2  1  0 |
// * |Reserved|Bank2 Aud|Reserved|Bank2 Vid|Reserved|Bank1 Aud|Reserved|Bank1 Vid|Reserved|Bank0 Aud|Reserved|Bank0 Vid|
enum TsScrmCtrlMode_t
{
  CA_PLAIN = 0x0,
  CA_SCRAMBLED1 = 0x1,
  CA_SCRAMBLED2 = 0x2,
  CA_SCRAMBLED3 = 0x3,
  CA_IDLE = 0x4,
  MAX_CA_SCRM_MODE = 0x5,
};

//
//#define REG_TSD_CA_CTRL_BANKMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+12*(f_num)))
//
///* jyp_hawk */
////#define REG_TSD_CA_CTRL_SLOTMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+36+4*(f_num)))
//#define REG_TSD_CA_CTRL_SLOTMODE(device_num,f_num) 			((TSD_CA_CTRL_BASE((device_num))+72+4*(f_num)))
//


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline uint32_t tsd_readl(
        struct sdp_tsd *tsd, u32 offset)
{
    uint32_t val = readl(tsd->regs + offset);
    //pr_err("#### tsd_readl(%08x) = %08x\n", 0x18100000 + offset, val);
    return val;
}

static inline void tsd_writel(
        struct sdp_tsd *tsd, u32 offset, u32 val)
{
    //pr_err("#### tsd_writel(%08x) = %08x\n", 0x18100000 + offset, val);
    writel(val, tsd->regs + offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __TSD_INTERNAL_H__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
