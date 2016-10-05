/*
 *  bugficks@samygo
 *	(c) 2016
 *
 *  License: GPLv3
 *
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_PREFIX " [SGO_DVB] "

#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/vt_kern.h>
#include <linux/security.h>

#include <linux/un.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/socket.h>

#include <linux/printk.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <../drivers/media/dvb-core/demux.h>
#include <../drivers/media/dvb-core/dmxdev.h>
#include <../drivers/media/dvb-core/dvb_ringbuffer.h>
#include <../drivers/media/dvb-core/dvb_demux.h>

#include <sdp_tsd/sdp_tsd_common.h>
#include <sdp_tsd/sdp_tsd_dvb.h>

#include "tsd_internal.h"
#include "sdp_util.h"

#include "llist.h"
#include "hijack.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//for some reason THIS_MODULE->version doesn't show "version" anymore
#define _MODULE_VERSION "0.5.0"

MODULE_DESCRIPTION("samygo dvb ca module");
MODULE_AUTHOR("bugficks@samygo");
MODULE_VERSION(_MODULE_VERSION);
MODULE_LICENSE("GPL");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int quiet = 0;
module_param(quiet, int, 0);
MODULE_PARM_DESC(quiet, "Less logging");

static int mmode = 0;
module_param(mmode, int, 0);
MODULE_PARM_DESC(mmode, "0: bank match (default), 1: pid match");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PATH_CA_SOCKET  "/tmp/sgoca.socket"

static DEFINE_SPINLOCK(lock_ca_sock);
static struct socket *CA_SOCK = 0;

static void send_ca_descr(ca_descr_t *cadescr, int bank);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    struct work_struct work;
    ca_descr_t ca_descr;
    int bank;
    struct list_head list;
} socket_worker_item_t;

static LIST_HEAD(worker_list_head);
static DEFINE_SPINLOCK(lock_socket_worker);
static struct workqueue_struct *socket_wq = 0;

static void socket_worker_func(
        struct work_struct *work)
{
    socket_worker_item_t *item = (socket_worker_item_t *)work;

    spin_lock(&lock_socket_worker);
    list_del(&item->list);
    spin_unlock(&lock_socket_worker);

    //pr_err(LOG_PREFIX">>>> socket_worker_func\n");
    send_ca_descr(&item->ca_descr, item->bank);
    //pr_err(LOG_PREFIX"<<<< socket_worker_func\n");

    kfree(work);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void send_ca_descr(
        ca_descr_t *cadescr, int bank)
{
    int i;
    
    spin_lock(&lock_ca_sock);

    for(i = 0; i < 2; i++)
    {
        int rr = 0;
        if(!CA_SOCK)
        {
            rr = sock_create(AF_UNIX, SOCK_STREAM, 0, &CA_SOCK);
            if(CA_SOCK)
            {
                struct sockaddr_un addr;
                memset(&addr, 0, sizeof(addr));
                addr.sun_family = AF_UNIX;
                strncpy(addr.sun_path, PATH_CA_SOCKET, sizeof(addr.sun_path) - 1);
                rr = sizeof(addr.sun_family) + (sizeof(PATH_CA_SOCKET) - 1);
                rr = kernel_connect(CA_SOCK, (struct sockaddr *)&addr, rr, 0);
            }
        }
        if(CA_SOCK)
        {
            int index = cadescr->index;
            cadescr->index = bank;
            rr = ktcp_send(CA_SOCK, cadescr, sizeof(ca_descr_t));
            cadescr->index = index;
            if(rr == sizeof(ca_descr_t))
                break;

            sock_release(CA_SOCK);
            CA_SOCK = 0;
        }
    }

    spin_unlock(&lock_ca_sock);
}

static void queue_send_ca_descr(
        ca_descr_t *cadescr, int bank)
{
    socket_worker_item_t *worker = kmalloc(sizeof(*worker), GFP_KERNEL);
    memset(worker, 0, sizeof(*worker));

    INIT_LIST_HEAD(&worker->list);
    INIT_WORK((struct work_struct *)worker, socket_worker_func);

    spin_lock(&lock_socket_worker);
    memcpy(&worker->ca_descr, cadescr, sizeof(worker->ca_descr));
    worker->bank = bank;

    list_add_tail(&worker->list, &worker_list_head);
    spin_unlock(&lock_socket_worker);

    queue_work(socket_wq, (struct work_struct *)worker);
}

static void queue_send_feed_state(
        enum tsd_bank bank, u8 state)
{
    uint32_t ioctl = 0;
    int do_send = 0;
    
    if(state == DMX_STATE_READY)
        ioctl = DMX_START;
    else if(state == DMX_STATE_GO)
        ioctl = DMX_STOP;

    if(ioctl)
    {
        static DEFINE_SPINLOCK(lock);
        static u8 bank_feed_state[TSD_BANK_MAX];
    
        spin_lock(&lock);
    
        if(bank_feed_state[bank] != state)
        {
            bank_feed_state[bank] = state;
            do_send = 1;
        }
    
        spin_unlock(&lock);
    }

    if(do_send)
    {
        struct ca_descr cadescr;
        cadescr.index = bank;
        cadescr.parity = ioctl;
        queue_send_ca_descr(&cadescr, bank);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TSD_CA_MODE_DISABLE     = 0,
    TSD_CA_MODE_BANK_MATCH  = 1,
    TSD_CA_MODE_PID_MATCH   = 2,
} tsd_ca_mode_t;

typedef struct
{
	tsd_ca_mode_t mode;
	dmx_ca_type_t ca_type;
	__u16 pid;	// in case of pid matching;
} tsd_ca_config_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Writing to key registers is restricted to TrustZone or im doing something wrong:)
static uint32_t tsd_set_csa_key(
        struct sdp_tsd *tsd, uint32_t cw_index, CaEvenOddType_t tCaEvenOddType, const uint8_t *pu8Key)
{
#if 0
    const uint32_t *pu32Key = (const uint32_t *)pu8Key;
    uint32_t cw0 = cpu_to_be32(pu32Key[0]);
    uint32_t cw1 = cpu_to_be32(pu32Key[1]);

    if(!tsd)
        return 0;

    if(tCaEvenOddType == EVEN_KEY)
    {
        pr_err(LOG_PREFIX"EVEN_KEY: offs_H: %08x offs_L: %08x ", REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_H(0, cw_index), REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_L(0, cw_index));
       
        tsd_writel(tsd, REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_H(0, cw_index), cw0);
        tsd_writel(tsd, REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_EVEN_L(0, cw_index), cw1);
    }
    else if(tCaEvenOddType == ODD_KEY)
    {
        pr_err(LOG_PREFIX"ODD_KEY: offs_H: %08x offs_L: %08x ", REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_H(0, cw_index), REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_L(0, cw_index));

        tsd_writel(tsd, REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_H(0, cw_index), cw0);
        tsd_writel(tsd, REG_TSD_CW_KEY_DES_MULTI2_BANK_MATCH_ODD_L(0, cw_index), cw1);
    }

    pr_err(LOG_PREFIX"cw0: %08x cw1: %08x\n", cw0, cw1);
#endif
    return 0;
}

static int tsd_ca_ctrl_cfg(
        struct sdp_tsd *tsd, enum tsd_bank bank, tsd_ca_config_t *cfg)
{
    unsigned long flags;

    if(cfg->mode == TSD_CA_MODE_DISABLE)
    {
        u32 v = tsd_readl(tsd, TSD_CA_CTRL(bank));
        v &= ~CA_EN;
        v &= ~CA_TYPE_MASK;
        tsd_writel(tsd, TSD_CA_CTRL(bank), v);

        if(cfg->pid != 0xFFFF)
        {
            int pid_idx;

            spin_lock_irqsave(&tsd->pid_lock, flags);
            pid_idx = find_pid_idx(tsd, cfg->pid, bank);
            spin_unlock_irqrestore(&tsd->pid_lock, flags);

            if(pid_idx >= 0)
            {
                u32 v1 = tsd_readl(tsd, TSD_PID(pid_idx));
                v1 &= ~PID_CA_TYPE_MASK;
                tsd_writel(tsd, TSD_PID(pid_idx), v1);
                tsd_writel(tsd, TSD_CA_CTRL(CA_CTRL_OFFSET + pid_idx), 0);
            }
        }
    }
    else if(cfg->mode == TSD_CA_MODE_BANK_MATCH)
    {
        u32 i;
        u32 v = CA_EN | CA_MATCH_TYPE | CA_BANK_SEL(bank) | CA_KEY_IV_INDEX(bank*3) | CA_IV_SEL(bank);
        v &= ~CA_TYPE_MASK;
        v |= CA_TYPE(cfg->ca_type);
        tsd_writel(tsd, TSD_CA_CTRL(bank), v);

        spin_lock_irqsave(&tsd->pid_lock, flags);

        for(i = 0; i < SDP_DMX_FILTER_MAX; i++)
        {
            struct sdp_pid_slot *pid_slot = &tsd->pid_slot[i];

            if(pid_slot->bank == bank)
            {
                u32 v1 = tsd_readl(tsd, TSD_PID(i));
                v1 &= ~PID_CA_TYPE_MASK;
                v1 |= PID_CA_TYPE(cfg->ca_type);
                tsd_writel(tsd, TSD_PID(i), v1);
                    
                // Clear pid matching
                tsd_writel(tsd, TSD_CA_CTRL(CA_CTRL_OFFSET + i), 0);
            }
        }

        spin_unlock_irqrestore(&tsd->pid_lock, flags);
    }
    else if(cfg->mode == TSD_CA_MODE_PID_MATCH)
    {
        int pid_idx;

        u32 v = CA_EN | CA_BANK_SEL(bank) | CA_KEY_IV_INDEX(bank*3) | CA_IV_SEL(bank);
        v &= ~CA_TYPE_MASK;
        v |= CA_TYPE(cfg->ca_type);

        spin_lock_irqsave(&tsd->pid_lock, flags);
        pid_idx = find_pid_idx(tsd, cfg->pid, bank);
        spin_unlock_irqrestore(&tsd->pid_lock, flags);
            
        if(pid_idx >= 0)
        {
            u32 v1 = tsd_readl(tsd, TSD_PID(pid_idx));
            v1 &= ~PID_CA_TYPE_MASK;
            v1 |= PID_CA_TYPE(cfg->ca_type);
            tsd_writel(tsd, TSD_PID(pid_idx), v1);

            v &= ~CA_PID_MASK;
            v |= CA_PID_INDEX(pid_idx);
            
            tsd_writel(tsd, TSD_CA_CTRL(CA_CTRL_OFFSET + pid_idx), v);

            // Clear bank matching
            tsd_writel(tsd, TSD_CA_CTRL(bank), 0);
        }
    }
    else
    {
        return -EINVAL;
    }

    return 0;
}

static int tsd_set_ca_bankmode(
        struct sdp_tsd *tsd, enum tsd_bank bank, enum tsd_ca_type ca_type)
{
    tsd_ca_config_t ca_config;
    memset(&ca_config, 0, sizeof(ca_config));

    ca_config.mode = ca_type == CA_BYPASS ? TSD_CA_MODE_DISABLE : TSD_CA_MODE_BANK_MATCH;
    ca_config.ca_type = ca_type;
    ca_config.pid = 0xFFFF;

    return tsd_ca_ctrl_cfg(tsd, bank, &ca_config);
}

static int dvb_ca_ioctl(
        struct dmx_demux *demux, unsigned int cmd, void *parg)
{
    struct dvb_demux *dvbdemux = (struct dvb_demux *)demux;
    struct sdp_bank *sb = (struct sdp_bank *)dvbdemux->priv;
    struct sdp_dvb *dvb = sb->dvb;
    struct sdp_tsd *tsd = &dvb->tsd;
    enum tsd_bank bank = sb->bank;

    enum tsd_bank_set set = get_bank_set_from_bank(bank);

    switch(cmd)
    {
        case CA_GET_CAP: 
        {
            // required for instant-on or if tvs-main-loop crashes
            struct ca_caps *caps = (struct ca_caps *)parg;

            caps->slot_num = 1;
            caps->slot_type = CA_CI_LINK;
            caps->descr_num = 0;
            caps->descr_type = 0;

            return 0;
        }

        case CA_SET_PID:
        {
            ca_pid_t *capid = (ca_pid_t*)parg;
            int pid_idx = 0;
            unsigned long flags = 0;

            spin_lock_irqsave(&tsd->pid_lock, flags);
            
            pid_idx = find_pid_idx(tsd, capid->pid, bank);
    
            spin_unlock_irqrestore(&tsd->pid_lock, flags);

            if(!quiet)
                pr_err(LOG_PREFIX"CA_SET_PID bank:%d capid->idx: %d capid->pid: %04x pid_idx: %d\n", bank, capid->index, capid->pid, pid_idx);

            if(pid_idx < 0)
                return 0;

            //print_filter_debug_info(tsd, pid_idx);

            {
                tsd_ca_config_t ca_cfg;
                memset(&ca_cfg, 0, sizeof(ca_cfg));
                ca_cfg.mode = capid->index == -1 ? TSD_CA_MODE_DISABLE : mmode;
                ca_cfg.ca_type = DMX_CA_DVB_CSA;
                ca_cfg.pid = capid->pid;
                tsd_ca_ctrl_cfg(tsd, bank, &ca_cfg);
            }
            
            sb->ca_type = DMX_CA_DVB_CSA;
            
            return 0;
        }
        case CA_SET_DESCR:
        {
            int do_send = 0;
            ca_descr_t *cadescr = (ca_descr_t*)parg;

            if(!quiet)
                pr_err(LOG_PREFIX"CA_SET_DESCR: bank: %d index: %d parity: %d cw: %*ph\n", bank, cadescr->index, cadescr->parity, 8, cadescr->cw);

            {
                static DEFINE_SPINLOCK(lock);
                static uint8_t __dcw[TSD_BANK_MAX][8 * 2];
                uint8_t *dcw = &__dcw[bank][sizeof(cadescr->cw) * (cadescr->parity & 1)];

                spin_lock(&lock);

                uint8_t *_dcw = cadescr->cw;
                _dcw[3] = _dcw[0] + _dcw[1] + _dcw[2];
                _dcw[7] = _dcw[4] + _dcw[5] + _dcw[6];

                if(memcmp(dcw, cadescr->cw, sizeof(cadescr->cw)))
                {
                    memcpy(dcw, cadescr->cw, sizeof(cadescr->cw));
                    do_send = 1;
                }

                spin_unlock(&lock);
            }
        
            if(do_send)
            {
                queue_send_ca_descr(cadescr, bank);
                //tsd_set_csa_key(tsd, 0, cadescr->parity == 0 ? EVEN_KEY : ODD_KEY, cadescr->cw);
            }
            return 0;
        }

        default:
            return -EINVAL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int (*dvb_demux_do_ioctl_t)(struct file *file, unsigned int cmd, void *parg);
static dvb_demux_do_ioctl_t dvb_demux_do_ioctl_org = 0;

static uintptr_t off_dvb_demux_do_ioctl = 0;
static int dvb_demux_do_ioctl_hook(
        struct file *file, unsigned int cmd, void *parg)
{
    int r = 0;
    struct dmxdev_filter *dmxdevfilter = file->private_data;
    struct dmxdev *dmxdev = dmxdevfilter->dev;

    if(mutex_lock_interruptible(&dmxdev->mutex))
        return -ERESTARTSYS;

    r = dvb_ca_ioctl(dmxdev->demux, cmd, parg);

    mutex_unlock(&dmxdev->mutex);

    if(r == -EINVAL)
        return dvb_demux_do_ioctl_org(file, cmd, parg);
    
    return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIJACK_IMPL_INIT(0x904, int, sdp_dvb_start_feed, struct dvb_demux_feed *feed)
{
    int r;
    HIJACK_LOCK(r, sdp_dvb_start_feed);
    {
        struct dvb_demux *demux = feed->demux;
        struct sdp_bank *sb = demux->priv;
        struct sdp_dvb *dvb = sb->dvb;
        struct sdp_tsd *tsd = &dvb->tsd;
        enum tsd_bank bank = sb->bank;

        //pr_err(LOG_PREFIX"sdp_dvb_start_feed: bank %d feed->type %d feed->state %d\n", bank, feed->type, feed->state);
        if(feed->pes_type == DMX_PES_PCR)
        {
            if(feed->ts_type == (TS_DECODER | TS_PACKET))
            {
                if(!quiet)
                    pr_err(LOG_PREFIX"sdp_dvb_start_feed(pvr): bank %d feed->ts_type %d feed->state %d pid: %04x\n", bank, feed->ts_type, feed->state, feed->pid);
                queue_send_feed_state(bank, feed->state);
            }
            else if(feed->ts_type == TS_DECODER)
            {
                if(!quiet)
                    pr_err(LOG_PREFIX"sdp_dvb_start_feed(live): bank %d feed->ts_type %d feed->state %d pid: %04x\n", bank, feed->ts_type, feed->state, feed->pid);
                queue_send_feed_state(bank, feed->state);
            }
        }

        HIJACK_DISPATCH(r, sdp_dvb_start_feed, feed);
    }
    HIJACK_UNLOCK(r, sdp_dvb_start_feed);
    
    return r;
}

HIJACK_IMPL_INIT(0x734, int, sdp_dvb_stop_feed, struct dvb_demux_feed *feed)
{
    int r;
    HIJACK_LOCK(r, sdp_dvb_stop_feed);
    {
        struct dvb_demux *demux = feed->demux;
        struct sdp_bank *sb = demux->priv;
        struct sdp_dvb *dvb = sb->dvb;
        struct sdp_tsd *tsd = &dvb->tsd;
        enum tsd_bank bank = sb->bank;

        if(feed->pes_type == DMX_PES_PCR)
        {
            if(feed->ts_type == (TS_DECODER | TS_PACKET))
            {
                if(!quiet)
                    pr_err(LOG_PREFIX"sdp_dvb_stop_feed(pvr): bank %d feed->ts_type %d feed->state %d pid: %04x\n", bank, feed->ts_type, feed->state, feed->pid);
                queue_send_feed_state(bank, feed->state);
            }
            else if(feed->ts_type == TS_DECODER)
            {
                if(!quiet)
                    pr_err(LOG_PREFIX"sdp_dvb_stop_feed(live): bank %d feed->ts_type %d feed->state %d pid: %04x\n", bank, feed->ts_type, feed->state, feed->pid);
                queue_send_feed_state(bank, feed->state);
            }
        }

        HIJACK_DISPATCH(r, sdp_dvb_stop_feed, feed);

    }
    HIJACK_UNLOCK(r, sdp_dvb_stop_feed);
    
    return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uintptr_t search_addr_dvb_demux_do_ioctl(void)
{
//HKM
//.text:C02A5C34 0D C0 A0 E1                 MOV             R12, SP
//.text:C02A5C38 F0 DF 2D E9                 STMFD           SP!, {R4-R12,LR,PC}
//.text:C02A5C3C 04 B0 4C E2                 SUB             R11, R12, #4
//.text:C02A5C40 14 D0 4D E2                 SUB             SP, SP, #0x14
//.text:C02A5C44 84 40 90 E5                 LDR             R4, [R0,#0x84]
//.text:C02A5C48 01 70 A0 E1                 MOV             R7, R1
//.text:C02A5C4C 02 50 A0 E1                 MOV             R5, R2
//.text:C02A5C50 71 A0 EF E6                 UXTB            R10, R1

//HKP
//.init:C02BEBEC 0D C0 A0 E1                 MOV             R12, SP
//.init:C02BEBF0 F0 DF 2D E9                 STMFD           SP!, {R4-R12,LR,PC}
//.init:C02BEBF4 04 B0 4C E2                 SUB             R11, R12, #4
//.init:C02BEBF8 14 D0 4D E2                 SUB             SP, SP, #0x14
//.init:C02BEBFC 84 40 90 E5                 LDR             R4, [R0,#0x84]
//.init:C02BEC00 01 70 A0 E1                 MOV             R7, R1
//.init:C02BEC04 02 50 A0 E1                 MOV             R5, R2
//.init:C02BEC08 71 A0 EF E6                 UXTB            R10, R1

// JAZZM
//.init:C0303010 0D C0 A0 E1                 MOV             R12, SP
//.init:C0303014 F0 DF 2D E9                 STMFD           SP!, {R4-R12,LR,PC}
//.init:C0303018 04 B0 4C E2                 SUB             R11, R12, #4
//.init:C030301C 1C D0 4D E2                 SUB             SP, SP, #0x1C
//.init:C0303020 84 40 90 E5                 LDR             R4, [R0,#0x84]
//.init:C0303024 01 70 A0 E1                 MOV             R7, R1
//.init:C0303028 02 A0 A0 E1                 MOV             R10, R2
//.init:C030302C 71 80 EF E6                 UXTB            R8, R1

    static const u8 search_pat_J[24] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0xF0, 0xDF, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x14, 0xD0, 0x4D, 0xE2,
        0x84, 0x40, 0x90, 0xE5, 0x01, 0x70, 0xA0, 0xE1,
    };
    static const u8 search_pat_K[24] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0xF0, 0xDF, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x1C, 0xD0, 0x4D, 0xE2,
        0x84, 0x40, 0x90, 0xE5, 0x01, 0x70, 0xA0, 0xE1,
    };

    struct 
    {
        uint32_t size;
        const u8 *pat;
        uintptr_t from;
    }
    search_pats[] = 
    {
#ifndef CONFIG_ARCH_SDP1501
        { sizeof(search_pat_J), search_pat_J, 0xC02A5000 },
#else
        { sizeof(search_pat_K), search_pat_K, 0xC0300000 },
#endif
    };

    static uintptr_t ploc = 0;
    if(ploc)
        return ploc;

    int j;
    for(j = 0; j < ARRAY_SIZE(search_pats); j++)
    {
        int i;
        uint8_t *start = (uint8_t*)search_pats[j].from;

        for(i = 0; i < 0x1A000; i += 4)
        {
            if(!memcmp(start + i, search_pats[j].pat, search_pats[j].size))
            {
                ploc = (uintptr_t)(start + i);
                return ploc;
            }

        }
    }

    return 0;
}

static uintptr_t search_addr_dvb_demux_ioctl_off_do_ioctl(void)
{
//HKM
//.text:C02A3E64             dvb_demux_ioctl                         ; DATA XREF: .text:C04473E4o
//.text:C02A3E64 0D C0 A0 E1                 MOV             R12, SP
//.text:C02A3E68 00 D8 2D E9                 STMFD           SP!, {R11,R12,LR,PC}
//.text:C02A3E6C 04 B0 4C E2                 SUB             R11, R12, #4
//.text:C02A3E70 04 30 9F E5                 LDR             R3, =dvb_demux_do_ioctl
//.text:C02A3E74 FA FD FF EB                 BL              dvb_usercopy
//.text:C02A3E78 00 A8 9D E8                 LDMFD           SP, {R11,SP,PC}
//.text:C02A3E78             ; End of function dvb_demux_ioctl
//.text:C02A3E78
//.text:C02A3E78             ; ---------------------------------------------------------------------------
//.text:C02A3E7C 34 5C 2A C0 off_C02A3E7C    DCD dvb_demux_do_ioctl  ; DATA XREF: dvb_demux_ioctl+Cr


// JAZZM
//.init:C0300B08 0D C0 A0 E1                 MOV             R12, SP
//.init:C0300B0C 00 D8 2D E9                 STMFD           SP!, {R11,R12,LR,PC}
//.init:C0300B10 04 B0 4C E2                 SUB             R11, R12, #4
//.init:C0300B14 10 30 03 E3+                MOV             R3, #dvb_demux_do_ioctl
//.init:C0300B1C 3D FD FF EB                 BL              dvb_usercopy
//.init:C0300B20 0C D0 4B E2                 SUB             SP, R11, #0xC
//.init:C0300B24 00 A8 9D E8                 LDMFD           SP, {R11,SP,PC}

    const u32 search_pat_J[] =
    {
        (u32)dvb_demux_do_ioctl_org,
    };
    const u32 search_pat_K[] =
    {
        arm_make_mov_rd(ARM_OP_MOVW, 3, ((u32)dvb_demux_do_ioctl_org) & 0xFFFF),
        arm_make_mov_rd(ARM_OP_MOVT, 3, ((u32)dvb_demux_do_ioctl_org >> 16) & 0xFFFF),
    };

    struct 
    {
        uint32_t size;
        const u32 *pat;
        uintptr_t from;
    }
    search_pats[] = 
    {
#ifndef CONFIG_ARCH_SDP1501
        { sizeof(search_pat_J), search_pat_J, 0x2000 },
#else
        { sizeof(search_pat_K), search_pat_K, 0x2700 },
#endif
    };

    static uintptr_t ploc = 0;
    if(ploc)
        return ploc;

    int j;
    for(j = 0; j < ARRAY_SIZE(search_pats); j++)
    {
        int i;
        uint8_t *start = (uint8_t*)dvb_demux_do_ioctl_org - search_pats[j].from;

        for(i = 0; i < 0x400; i += 4)
        {
            if(!memcmp(start + i, search_pats[j].pat, search_pats[j].size))
            {
                ploc = (uintptr_t)(start + i);
                return ploc;
            }

        }
    }
    return 0;
}

static uintptr_t search_addr_sdp_dvb_start_feed(void)
{
//HKM
//.text:00000904 0D C0 A0 E1                 MOV             R12, SP
//.text:00000908 30 D8 2D E9                 STMFD           SP!, {R4,R5,R11,R12,LR,PC}
//.text:0000090C 04 B0 4C E2                 SUB             R11, R12, #4
//.text:00000910 10 D0 4D E2                 SUB             SP, SP, #0x10
//.text:00000914 F8 30 01 E3                 MOV             R3, #0x10F8
//.text:00000918 00 40 A0 E1                 MOV             R4, R0
//.text:0000091C 03 30 90 E7                 LDR             R3, [R0,R3]

// JAZZM
//.text:00000ED0 0D C0 A0 E1                 MOV             R12, SP
//.text:00000ED4 F0 D8 2D E9                 STMFD           SP!, {R4-R7,R11,R12,LR,PC}
//.text:00000ED8 04 B0 4C E2                 SUB             R11, R12, #4
//.text:00000EDC 10 D0 4D E2                 SUB             SP, SP, #0x10
//.text:00000EE0 01 4A 80 E2                 ADD             R4, R0, #0x1000
//.text:00000EE4 F8 30 94 E5                 LDR             R3, [R4,#0xF8]
//.text:00000EE8 04 C0 93 E5                 LDR             R12, [R3,#4]

    static const u8 search_pat_J[20] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0x30, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x10, 0xD0, 0x4D, 0xE2,
        0xF8, 0x30, 0x01, 0xE3,
    };
    static const u8 search_pat_K[20] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0xF0, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x10, 0xD0, 0x4D, 0xE2, 
        0x01, 0x4A, 0x80, 0xE2, 
    };

    struct 
    {
        uint32_t size;
        const u8 *pat;
        uintptr_t from;
    }
    search_pats[] = 
    {
#ifndef CONFIG_ARCH_SDP1501
        { sizeof(search_pat_J), search_pat_J, 0x800 },
#else
        { sizeof(search_pat_K), search_pat_K, 0xD00 },
#endif
    };

    static uintptr_t ploc = 0;
    if(ploc)
        return ploc;

    int j;
    for(j = 0; j < ARRAY_SIZE(search_pats); j++)
    {
        int i;
        uint8_t *start = (uint8_t*)mod_ADDR("sdp_tsd", search_pats[j].from);

        for(i = 0; i < 0x200; i += 4)
        {
            if(!memcmp(start + i, search_pats[j].pat, search_pats[j].size))
            {
                ploc = (uintptr_t)(start + i);
                return ploc;
            }
        }
    }
    return 0;
}

static uintptr_t search_addr_sdp_dvb_stop_feed(void)
{
//HKM
//.text:00000734 0D C0 A0 E1                 MOV             R12, SP
//.text:00000738 10 D8 2D E9                 STMFD           SP!, {R4,R11,R12,LR,PC}
//.text:0000073C 04 B0 4C E2                 SUB             R11, R12, #4
//.text:00000740 0C D0 4D E2                 SUB             SP, SP, #0xC
//.text:00000744 F8 20 01 E3                 MOV             R2, #0x10F8
//.text:00000748 00 30 A0 E1                 MOV             R3, R0

//JAZZM
//.text:00000CE4 0D C0 A0 E1                 MOV             R12, SP
//.text:00000CE8 30 D8 2D E9                 STMFD           SP!, {R4,R5,R11,R12,LR,PC}
//.text:00000CEC 04 B0 4C E2                 SUB             R11, R12, #4
//.text:00000CF0 08 D0 4D E2                 SUB             SP, SP, #8
//.text:00000CF4 01 3A 80 E2                 ADD             R3, R0, #0x1000
//.text:00000CF8 F8 C0 93 E5                 LDR             R12, [R3,#0xF8]

    static const u8 search_pat_J[20] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0x10, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x0C, 0xD0, 0x4D, 0xE2,
        0xF8, 0x20, 0x01, 0xE3,
    };
    const u8 search_pat_K[20] =
    {
        0x0D, 0xC0, 0xA0, 0xE1, 0x30, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x08, 0xD0, 0x4D, 0xE2, 
        0x01, 0x3A, 0x80, 0xE2, 
    };

    struct 
    {
        uint32_t size;
        const u8 *pat;
        uintptr_t from;
    }
    search_pats[] = 
    {
#ifndef CONFIG_ARCH_SDP1501
        { sizeof(search_pat_J), search_pat_J, 0x700 },
#else
        { sizeof(search_pat_K), search_pat_K, 0xB00 },
#endif
    };

    static uintptr_t ploc = 0;
    if(ploc)
        return ploc;

    int j;
    for(j = 0; j < ARRAY_SIZE(search_pats); j++)
    {
        int i;
        uint8_t *start = (uint8_t*)mod_ADDR("sdp_tsd", search_pats[j].from);
        static uintptr_t ploc = 0;
        if(ploc)
            return ploc;

        for(i = 0; i < 0x200; i += 4)
        {
            if(!memcmp(start + i, search_pats[j].pat, search_pats[j].size))
            {
                ploc = (uintptr_t)(start + i);
                return ploc;
            }
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int __init _module_init(void)
{
    pr_err(LOG_PREFIX"=============================================================\n");
    pr_err(LOG_PREFIX"%s v%s - (c) bugficks 2016\n", THIS_MODULE->name, _MODULE_VERSION);

    // some compile time checks to make sure stuff matches
    {
#ifdef CONFIG_ARCH_SDP1406
        struct tsd_global_resource _test;
        _test.g_tsd_top_reg5 = 0;
#endif
#ifndef CONFIG_ARCH_SDP1501
        BUILD_BUG_ON(offsetof(struct dmxdev, mutex) != 136);
#endif
    }

    dvb_demux_do_ioctl_org = (dvb_demux_do_ioctl_t)search_addr_dvb_demux_do_ioctl();
    if(!dvb_demux_do_ioctl_org)
    {
        pr_err(LOG_PREFIX"Error getting address of dvb_demux_do_ioctl\n");
        return -EINVAL;
    }
    off_dvb_demux_do_ioctl = search_addr_dvb_demux_ioctl_off_do_ioctl();
    if(!off_dvb_demux_do_ioctl)
    {
        pr_err(LOG_PREFIX"Error getting address of off_dvb_demux_do_ioctl\n");
        return -EINVAL;
    }

    sdp_dvb_start_feed_org = (sdp_dvb_start_feed_t)search_addr_sdp_dvb_start_feed();
    if(!sdp_dvb_start_feed_org)
    {
        pr_err(LOG_PREFIX"Error getting address of sdp_dvb_start_feed\n");
        return -EINVAL;
    }
    sdp_dvb_stop_feed_org = (sdp_dvb_stop_feed_t)search_addr_sdp_dvb_stop_feed();
    if(!sdp_dvb_stop_feed_org)
    {
        pr_err(LOG_PREFIX"Error getting address of sdp_dvb_stop_feed\n");
        return -EINVAL;
    }

    if(mmode == 1)
    {
        mmode = TSD_CA_MODE_PID_MATCH;
        pr_err(LOG_PREFIX"TSD_CA_MODE_PID_MATCH\n");
    }
    else
    {
        mmode = TSD_CA_MODE_BANK_MATCH;
        pr_err(LOG_PREFIX"TSD_CA_MODE_BANK_MATCH\n");
    }

    pr_err(LOG_PREFIX"dvb_demux_do_ioctl: %p\n", (void*)dvb_demux_do_ioctl_org);
    pr_err(LOG_PREFIX"off_dvb_demux_do_ioctl : %p [%*ph]\n", (void*)off_dvb_demux_do_ioctl, 8, (void*)off_dvb_demux_do_ioctl);
    pr_err(LOG_PREFIX"sdp_dvb_start_feed: %p\n", (void*)sdp_dvb_start_feed_org);
    pr_err(LOG_PREFIX"sdp_dvb_stop_feed: %p\n", (void*)sdp_dvb_stop_feed_org);

    socket_wq = create_workqueue("socket_wq");
    if(!socket_wq)
    {
        pr_err(LOG_PREFIX"Error create_workqueue.\n");
        return -EINVAL;
    }

    {
        u8 *p = (u8*)off_dvb_demux_do_ioctl;
        if(p[3] == 0xE3 && p[7] == 0xE3)
        {
            u8 rd = p[1] >> 8;
            u32 patch[] = 
            {
                arm_make_mov_rd(ARM_OP_MOVW, rd, ((u32)dvb_demux_do_ioctl_hook) & 0xFFFF),
                arm_make_mov_rd(ARM_OP_MOVT, rd, ((u32)dvb_demux_do_ioctl_hook >> 16) & 0xFFFF),
            };
            memcpy((void*)off_dvb_demux_do_ioctl, patch, sizeof(patch));
            cacheflush((void*)off_dvb_demux_do_ioctl, sizeof(patch));
        }
        else
        {
            *((dvb_demux_do_ioctl_t*)off_dvb_demux_do_ioctl) = dvb_demux_do_ioctl_hook;
            cacheflush((void*)off_dvb_demux_do_ioctl, sizeof(dvb_demux_do_ioctl_org));
        }
    }
 
    HIJACK_START(sdp_dvb_start_feed);
    HIJACK_START(sdp_dvb_stop_feed);

    pr_err(LOG_PREFIX"=============================================================\n");

    return 0;

}

static void __exit _module_exit(void)
{
    pr_err(LOG_PREFIX"=============================================================\n");
    pr_err(LOG_PREFIX"%s exiting\n", THIS_MODULE->name);

    if(socket_wq)
    {
        flush_workqueue(socket_wq);
        destroy_workqueue(socket_wq);
        socket_wq = 0;
    }

    struct list_head *node = 0, *nodetmp = 0;
    list_for_each_safe(node, nodetmp, &worker_list_head)
    {
        socket_worker_item_t *item = list_entry(node, socket_worker_item_t, list);
        if(item)
        {
            list_del(&item->list);
            kfree(item);
        }
    }

    if(CA_SOCK)
    {
        sock_release(CA_SOCK);
        CA_SOCK = 0;
    }

    {
        u8 *p = (u8*)off_dvb_demux_do_ioctl;
        if(p[3] == 0xE3 && p[7] == 0xE3)
        {
            u8 rd = p[1] >> 8;
            u32 patch[] = 
            {
                arm_make_mov_rd(ARM_OP_MOVW, rd, ((u32)dvb_demux_do_ioctl_org) & 0xFFFF),
                arm_make_mov_rd(ARM_OP_MOVT, rd, ((u32)dvb_demux_do_ioctl_org >> 16) & 0xFFFF),
            };
            memcpy((void*)off_dvb_demux_do_ioctl, patch, sizeof(patch));
            cacheflush((void*)off_dvb_demux_do_ioctl, sizeof(patch));
        }
        else
        {
            *((dvb_demux_do_ioctl_t*)off_dvb_demux_do_ioctl) = dvb_demux_do_ioctl_org;
            cacheflush((void*)off_dvb_demux_do_ioctl, sizeof(dvb_demux_do_ioctl_org));
        }
    }

    HIJACK_STOP(sdp_dvb_start_feed);
    HIJACK_STOP(sdp_dvb_stop_feed);

    pr_err(LOG_PREFIX"=============================================================\n");
}

module_init(_module_init);
module_exit(_module_exit);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
