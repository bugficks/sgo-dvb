/*
 *  bugficks
 *	(c) 2013 - inf
 *
 *  License: GPLv3
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __UTILS_H__
#define __UTILS_H__

//////////////////////////////////////////////////////////////////////////

#include <linux/version.h>
#include <linux/module.h>

//////////////////////////////////////////////////////////////////////////

#ifndef LOG_PREFIX
#define LOG_PREFIX " [SamyGO]"
#endif

//////////////////////////////////////////////////////////////////////////

#define secs_to_jiffies(s)  msecs_to_jiffies(1000 * s)

//////////////////////////////////////////////////////////////////////////

#define ARM_NOP             0xE1A00000
#define ARM_MOV_R12_SP      0xE1A0C00D
#define ARM_MOV_R3_0x350    0xE3A03E35
#define ARM_ORR_R10_R10_4   0xE38AA004

#define ARM_MOV_R3_0x381    0xE3003381
#define ARM_ORR_R5_R5_7     0xE3855007

//////////////////////////////////////////////////////////////////////////

typedef enum
{
    ARM_OP_MOV      = 0xE3A,
    ARM_OP_MOVW     = 0xE30,
    ARM_OP_MVN      = 0xE3E,
    ARM_OP_MOVT     = 0xE34,
    ARM_OP_MOVEQ    = 0x030
} ARM_OP_MOV_t;

static inline uint32_t arm_make_mov_rd(
        uint16_t op, uint8_t rd, uint16_t val)
{
    //uint32_t mov_imm16 = op << 24;
    uint32_t mov_imm16 = op << 20;
    uint32_t reg = rd << 12;
    mov_imm16 = mov_imm16 | reg;

    uint32_t imm16hi = (val & 0xf000) << 4;
    uint32_t imm16lo = val & 0x0fff;

    uint32_t ret = mov_imm16 | imm16hi | imm16lo;
    return ret;
}

//////////////////////////////////////////////////////////////////////////

uint32_t *do_patch(
        bool set, const char *lbl, uint32_t *p, uint32_t *insn_new, uint32_t *insn_save,
        uint32_t cnt);

void cacheflush ( void *begin, unsigned long size );

//////////////////////////////////////////////////////////////////////////

int task_kill(
        const char *task_name);
bool is_module_loaded(
        const char *name);
int mod_delete_module(
        const char *modname);
long mod_mount(
        const char *dev_name, const char *dir_name, const char *type, unsigned long flags, void *data);


unsigned long **aquire_sys_call_table(void);

//////////////////////////////////////////////////////////////////////////

void *mod_ADDR(const char *name, uint32_t offs);

//////////////////////////////////////////////////////////////////////////

int ktcp_send(struct socket *sock, const void *buf, int len);
int ktcp_recv(struct socket *sock, void *buf, int len);

//////////////////////////////////////////////////////////////////////////

#define IS_KVER_GE(...) \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(__VA_ARGS__))

#define IS_KVER_GE_v3 IS_KVER_GE(3,0,0)
#define IS_KVER_GE_v3x8 IS_KVER_GE(3,8,0)

//////////////////////////////////////////////////////////////////////////


#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N

#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

//////////////////////////////////////////////////////////////////////////

#endif // #ifndef __UTILS_H__

//////////////////////////////////////////////////////////////////////////
