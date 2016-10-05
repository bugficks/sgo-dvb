/*
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 *  bugficks@samygo 
 *      Copied and modified required stuff from sdp_tsd sources
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <linux/io.h>
#include <linux/platform_device.h>

#include "sdp_util.h"
#include "tsd_internal.h"

//////////////////////////////////////////////////////////////////////////

int find_pid_idx(
    struct sdp_tsd *tsd, u16 pid, enum tsd_bank bank)
{
    struct sdp_pid_slot *pid_slot;
    int i;

    for (i = 0; i < SDP_DMX_FILTER_MAX; i++) {
        pid_slot = &tsd->pid_slot[i];

        if ((pid_slot->pid == pid) && (pid_slot->bank == bank)) {
            return i;
        }
    }

    return -1;
}

void print_filter_debug_info(
    struct sdp_tsd *tsd, u32 pid_idx)
{
    u32 v;
    u32 v1;

    v = tsd_readl(tsd, TSD_PID(pid_idx));
    pr_err("(Bank%d) PID(%d) = 0x%x enabled = %d\n", PID_BANK(v), pid_idx, PID_PID(v), (v & PID_PID_EN)?1:0);
    if( v & PID_PID_EN ) {
        //pr_err(" PID EN\n");
        if(v & PID_PPE_IND)
            pr_err("VIDEO IND EN\n");
        if(v & PID_DMX_EN)
            pr_err(" DMX_EN\n");
        if(v & PID_PVR_EN)
            pr_err(" PVR_EN\n");
        if(v & PID_H264_OUT_EN)
            pr_err(" 264_EN\n");
        if(v & PID_BYPASS_EN)
            pr_err(" BYPASS_EN\n");

        pr_err(" CA TYPE  = %d\n",((v>>22) & 0xf));

        pr_err(" TARGET_AREA=");
        v1 =  tsd_readl(tsd, SW_PCONT(pid_idx));
        switch((v1>>25) & 0xf)
        {
            case AREA_VIDEO0:
                pr_err("Video0 : old mpeg(not supported)\n");
                break;
            case AREA_VIDEO1:
                pr_err("Video1 : mfd0\n");
                break;
            case AREA_AUDIO0:
                pr_err("Audio0\n");
                break;
            case AREA_AUDIO1:
                pr_err("Audio1\n");
                break;
            case AREA_AUDIO2:
                pr_err("Audio2\n");
                break;
            case AREA_SECTION:
                pr_err("Section\n");
                break;
            case AREA_VIDEO2:
                pr_err("Video2 : mfd1\n");
                break;
            case AREA_VIDEO4:
                pr_err("Video4 : hevc\n");
                break;
            case AREA_VIDEO5:
                pr_err("Video5 : uddec\n");
                break;
            default :
                pr_err(" target_area.resrv\n");
                break;
        }
        pr_err(" DEMUX_MODE=%d", (v1) & 0xf );
        pr_err(" QUEUE_SIZE=%dK\n",((v1>>12) & 0x3ff) * 4 );

        if (v1 & PCONT_PCR_EN)
            pr_err(" PCR EN\n");
        if(v1 & PCONT_CLEAR_P_STATE)
            pr_err(" CLEAR_P_STATE\n");
        if(v1 & PCONT_UPDATE_PCR)
            pr_err(" UPDATE_PCR\n");
        if(v1 & PCONT_LOAD_PCR)
            pr_err(" LOAD_PCR\n");
        if(v1 & PCONT_PCR_ONE_SHOT)
            pr_err(" PCR_ONE_SHOT\n");
        if(v1 & PCONT_IS_BLOCKING_DMA)
            pr_err(" BLOCKING_DMA\n");

        pr_err("Queue From=0x%X Wr=0x%X To=0x%X\n"
                , tsd_readl(tsd, SW_Q_START(pid_idx))
                , tsd_readl(tsd, SW_Q_PKT(pid_idx))
                , tsd_readl(tsd, SW_Q_SEC(pid_idx)));
        pr_err("\n");


        if(v1>>30 & 0x3) {
            switch((v1>>30) & 0x3)
            {
                case 1:
                    pr_err("Second Target HEVC\n");
                    pr_err("Queue From=0x%X Wr=0x%X CLK=0x%X\n"
                            , tsd_readl(tsd, SW_SECOND_Q_START(0))
                            , tsd_readl(tsd, SW_SECOND_Q_WRITE(0))
                            , tsd_readl(tsd, SW_DEC_CLK_ENABLE_STS) );
                    pr_err(" QUEUE_SIZE=%dK\n", tsd_readl(tsd, SW_SECOND_TARGET_CTRL(0)) );
                    break;
                case 2:
                    pr_err("Second Target UDDEC\n");
                    pr_err("Queue From=0x%X Wr=0x%X CLK=0x%X\n"
                            , tsd_readl(tsd, SW_SECOND_Q_START(1))
                            , tsd_readl(tsd, SW_SECOND_Q_WRITE(1))
                            , tsd_readl(tsd, SW_DEC_CLK_ENABLE_STS) );
                    pr_err(" QUEUE_SIZE=%dK\n", tsd_readl(tsd, SW_SECOND_TARGET_CTRL(1)) );
                    break;
                default :
                    pr_err(" target_area.resrv\n");
                    break;
            }
        }
    }
}

void print_all_filter_debug_info(struct sdp_tsd *tsd)
{
    struct sdp_pid_slot *pid_slot;
    int i;

    pr_err("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    for(i = 0; i < SDP_DMX_FILTER_MAX; i++)
    {
        pid_slot = &tsd->pid_slot[i];
        if(pid_slot->pid == PID_NULL)
            continue;

        print_filter_debug_info(tsd, i);
    }
    pr_err("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void get_bank_router_info( struct sdp_tsd *tsd, enum tsd_bank bank)
{
    u32 v;
    enum tsd_iface inf;
    v = tsd_readl(tsd, TSD_PID_IN_SEL);
    inf  = (v>>((bank)>>2)) & 0x7;
    pr_err(" Interface  = %d\n",inf);
}

//////////////////////////////////////////////////////////////////////////
