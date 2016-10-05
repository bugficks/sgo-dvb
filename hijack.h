/*
 *  https://github.com/mncoppola/suterusu
 *
 *  License: MIT
 *
 */
//////////////////////////////////////////////////////

#ifndef __HOOK_H__
#define __HOOK_H__

//////////////////////////////////////////////////////

#ifndef LOG_PREFIX
#define LOG_PREFIX ""
#endif

//////////////////////////////////////////////////////

#define HIJACK_DISPATCH_STD(RET, F, ...) \
    hijack_pause(F##_org); \
    RET = (F##_org)(__VA_ARGS__); \
    hijack_resume(F##_org);

#define HIJACK_DISPATCH HIJACK_DISPATCH_STD

#define HIJACK_LOCK(RET, NAME) \
    static DEFINE_MUTEX(__mtx_##NAME); \
    RET = -ERESTARTSYS; \
    if(!mutex_lock_interruptible(&__mtx_##NAME)) \
    { 

#define HIJACK_UNLOCK(RET, NAME) \
        mutex_unlock(&__mtx_##NAME); \
    }

#define HIJACK_IMPL_INIT(ADDR, RET, NAME, ...) \
    typedef RET (* NAME##_t)(__VA_ARGS__); \
    static NAME##_t NAME##_org = (NAME##_t)ADDR; \
    static RET NAME##_hook(__VA_ARGS__)

#define HIJACK_IMPL(RET, NAME, ...) \
    HIJACK_IMPL_INIT(0, RET, NAME, __VA_ARGS__)


#define HIJACK_START(NAME) \
    if(NAME##_org) {\
        pr_err(LOG_PREFIX"Hooking " #NAME ": %p\n", (void*)NAME##_org);  \
        hijack_start(NAME##_org, &NAME##_hook); \
    }

#define HIJACK_STOP(NAME) \
    if(NAME##_org) {\
        pr_err(LOG_PREFIX"Unhooking " #NAME ": %p\n", (void*)NAME##_org);  \
        hijack_stop(NAME##_org); \
    }

//////////////////////////////////////////////////////

void hijack_start ( void *target, void *new );
void hijack_pause ( void *target );
void hijack_resume ( void *target );
void hijack_stop ( void *target );
bool hijack_is_hijacked ( void *target );

//////////////////////////////////////////////////////

#endif // #ifndef __HOOK_H__

//////////////////////////////////////////////////////
