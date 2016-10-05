/*
 *  bugficks
 *	(c) 2013-inf
 *
 *  License: GPLv3
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>

#include <linux/un.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/socket.h>

#include "utils.h"
#include "llist.h"

//////////////////////////////////////////////////////////////////////////

#define LOG_ERR    KERN_ERR LOG_PREFIX

//////////////////////////////////////////////////////////////////////////

void cacheflush ( void *begin, unsigned long size )
{
    flush_icache_range((uintptr_t)begin, (uintptr_t)begin + size);
}

//////////////////////////////////////////////////////////////////////////

unsigned long **aquire_sys_call_table(void)
{
    uintptr_t offset = PAGE_OFFSET;
    static unsigned long **sct = 0;
    if(sct)
        return sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if((sct[__NR_close] == (unsigned long *) sys_close)
            && (sct[__NR_unlink] == (unsigned long *) sys_unlink)
            && (sct[__NR_mknod] == (unsigned long *) sys_mknod))
                return sct;

        offset += sizeof(void *);
    }
    sct = 0;

    return sct;
}

int task_kill(
        const char *task_name)
{
    typedef struct task_list_entry
    {
        struct llist_node list;
        struct task_struct *tsk;
    } task_list_entry_t;

    struct task_struct *tsk = 0;
    int ret = -1;
    int task_name_len = strlen(task_name);
    LLIST_HEAD(tsk_list);
    int cnt = 0;

    //read_lock(&tasklist_lock);
    rcu_read_lock();
    for_each_process(tsk)
    {
        task_lock(tsk);
        //struct pid *pid = task_pid(tsk);
        // printk(KERN_ERR"*** task: %s pid: %d, kt: %d\n", tsk->comm, tsk->pid, tsk->flags & PF_KTHREAD);
        if((tsk->flags & PF_EXITING) == 0)
        {
            if(!strncmp(tsk->comm, task_name, task_name_len))
            {
                task_list_entry_t *tle = kmalloc(sizeof(task_list_entry_t), GFP_KERNEL);
                if(!tle)
                    panic("can't allocate task_list_entry_t buffer");

                tle->tsk = tsk;
                llist_add(&tle->list, &tsk_list);
            }
        }
        task_unlock(tsk);
    }
    rcu_read_unlock();
    //read_unlock(&tasklist_lock);

    if(!llist_empty(&tsk_list))
    {
        struct llist_node *head = llist_del_all(&tsk_list);
        while(head)
        {
            task_list_entry_t *node = llist_entry(head, task_list_entry_t, list);
            head = llist_next(head);
            if(node)
            {
                struct task_struct *tsk_tokill = node->tsk;
                kfree(node);

                get_task_struct(tsk_tokill);

                if(tsk_tokill->flags & PF_KTHREAD)
                    ret = kthread_stop(tsk_tokill);
                else
                    ret = send_sig(SIGKILL, tsk_tokill, 0);

                put_task_struct(tsk_tokill);

                printk(LOG_ERR" task_kill(%s): %d\n", task_name, ret);

                if(ret >= 0)
                    cnt++;
            }
        }
    }

    return cnt;
}

bool is_module_loaded(
        const char *name)
{
    struct module *mod = 0;

    mutex_lock(&module_mutex);
    mod = find_module(name);
    mutex_unlock(&module_mutex);

    return (mod != 0);
}

//////////////////////////////////////////////////////////////////////////

int mod_delete_module(
        const char *modname)
{
    mm_segment_t old_fs;
    int ret = -1;
    unsigned long **sct = aquire_sys_call_table();

    typedef long (*delete_module_t)(const char __user *name_user, unsigned int flags);
    static delete_module_t delete_module = 0;
    if(!delete_module)
        delete_module = (delete_module_t)sct[__NR_delete_module];

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    //set_fs(get_ds());

    ret = delete_module(modname, O_TRUNC);
    printk(LOG_ERR" delete_module(%s) -> %d\n", modname, ret);

    set_fs(old_fs);

    return ret;
}

int mod_touch(
        const char *path)
{
    struct file *filp = filp_open(path, O_WRONLY|O_CREAT, 0);
    printk(LOG_ERR" mod_touch(%s) -> %p \n", path, filp);
    if(IS_ERR(filp))
        return PTR_ERR(filp);

    filp_close(filp, 0);
    return 0;
}

int mod_unlink(
        const char *path)
{
    mm_segment_t old_fs;
    int ret = -1;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_unlink(path);
    printk(LOG_ERR" mod_unlink(%s) -> %d \n", path, ret);

    set_fs(old_fs);

    return ret;
}

long mod_mount(
        const char *dev_name, const char *dir_name, const char *type, unsigned long flags, void *data)
{
    typedef long (*sys_mount_t)(const char *dev_name, const char *dir_name, const char *type, unsigned long flags, void __user *data);

    int err;
    mm_segment_t old_fs;
    sys_mount_t mount = 0;

    unsigned long **sct = aquire_sys_call_table();
    if(!sct)
        return -EINVAL;

    mount = (sys_mount_t)sct[__NR_mount];
    if(!mount)
        return -EINVAL;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    err = mount(dev_name, dir_name, type, flags, data);

    set_fs(old_fs);

    return err;
}

//////////////////////////////////////////////////////////////////////////

uint32_t *do_patch(
        bool set, const char *lbl, uint32_t *p, uint32_t *insn_new, uint32_t *insn_save,
        uint32_t cnt)
{
    uint32_t ncopy = sizeof(*insn_new) * cnt;
    if(!p)
    {
        printk(LOG_ERR" %s patch location not found.\n", lbl);
        return 0;
    }

    //printk(KERN_ERR" *** %s *** @ 0x%p 0x%08x.\n", lbl, p, p[0]);

    if(set)
    {
        if(memcmp(p, insn_new, ncopy))
        {
            printk(LOG_ERR" patching %s [0x%p : %08x = %08x]\n", lbl, p, p[0], insn_new[0]);
            memcpy(insn_save, p, ncopy);
            memcpy(p, insn_new, ncopy);
            cacheflush(p, ncopy);
        }
        else
        {
            printk(LOG_ERR" %s already patched [0x%p : %08x]\n", lbl, p, p[0]);
        }
    }
    else
    {
        printk(LOG_ERR" reverting %s [0x%p : %08x = %08x]\n", lbl, p, p[0], insn_save[0]);
        memcpy(p, insn_save, ncopy);
        cacheflush(p, 4);
    }

    return p;
}

//////////////////////////////////////////////////////////////////////////

static void *resolve_symbol(char *name)
{
    void *ret = 0;
    const struct kernel_symbol *sym;

    mutex_lock(&module_mutex);
    sym = find_symbol(name, NULL, NULL, 1, true);
    mutex_unlock(&module_mutex);

    if (sym) {
        ret = (void *)sym->value;
    }

    return ret;
}

void *mod_ADDR(const char *name, uint32_t offs)
{
    ptrdiff_t mod_addr = 0;
    struct module *mod = find_module(name);
    if(mod)
        mod_addr = (ptrdiff_t)mod->module_core;

    //printk(KERN_ERR"**** %s: %08x\n", name, mod_addr);

    if(!mod_addr)
        return 0;

    return (void*)(mod_addr + offs);
}

//////////////////////////////////////////////////////////////////////////
// https://gist.github.com/llj098/752417

int ktcp_send(struct socket *sock, const void *buf, int len)
{
	if(sock==NULL)
		return -1;

    struct msghdr msg;
	struct iovec iov;
	int size;
	mm_segment_t oldfs;

	iov.iov_base=(void*)buf;
	iov.iov_len=len;

	msg.msg_control=NULL;
	msg.msg_controllen=0;
	msg.msg_flags=0;
	msg.msg_iov=&iov;
	msg.msg_iovlen=1;
	msg.msg_name=0;
	msg.msg_namelen=0;

	oldfs=get_fs();
	set_fs(KERNEL_DS);
	size=sock_sendmsg(sock,&msg,len);
	set_fs(oldfs);

	return size;
}

int ktcp_recv(struct socket *sock, void *buf, int len)
{
	if(sock==NULL)
        return -1;

	struct msghdr msg;
	struct iovec iov;
	mm_segment_t oldfs;
	int size=0;

	if(sock->sk==NULL)
        return 0;

	iov.iov_base=buf;
	iov.iov_len=len;

	msg.msg_control=NULL;
	msg.msg_controllen=0;
	msg.msg_flags=0;
	msg.msg_name=0;
	msg.msg_namelen=0;
	msg.msg_iov=&iov;
	msg.msg_iovlen=1;

    oldfs=get_fs();
	set_fs(KERNEL_DS);
	size=sock_recvmsg(sock,&msg,len,msg.msg_flags);
	set_fs(oldfs);

	return size;
}
//////////////////////////////////////////////////////////////////////////
