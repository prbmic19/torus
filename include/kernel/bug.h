#ifndef KERNEL_BUG_H
#define KERNEL_BUG_H

#include <torus/compiler.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>

// Must define struct regs, context_store(), and backtrace_dump_warn().
#include <asm/regs.h>

/*
 * USE BUG() AND BUG_ON() SPARINGLY!
 * These terminate the kernel abruptly and should be used only when
 * continuing execution would almost certainly corrupt the kernel state.
 *
 * Prefer normal error handling or WARN*() whenever recovery is possible.
 */

#define BUG()                 \
    do                        \
    {                         \
        struct regs regs;     \
        context_store(&regs); \
        panic(&regs, "Aieee, kernel BUG at %s:%d/%s()!", __FILE__, __LINE__, __func__); \
    }                         \
    while (0)

#define BUG_ON(cond)        \
    do                      \
    {                       \
        if (unlikely(cond)) \
        {                   \
            BUG();          \
        }                   \
    }                       \
    while (0)


#define WARN(cond, fmt, ...) ({          \
        bool __ret_cond = !!(cond);      \
        if (unlikely(__ret_cond))        \
        {                                \
            pr_warn(fmt, ##__VA_ARGS__); \
            backtrace_dump_warn((unsigned long)__builtin_frame_address(0)); \
        }                                \
        unlikely(__ret_cond);            \
    })

#define WARN_ON(cond) ({            \
        bool __ret_cond = !!(cond); \
        if (unlikely(__ret_cond))   \
        {                           \
            pr_warn("WARN at %s:%d:%s().\n", __FILE__, __LINE__, __func__); \
            backtrace_dump_warn((unsigned long)__builtin_frame_address(0)); \
        }                           \
        unlikely(__ret_cond);       \
    })

#define WARN_ONCE(cond, fmt, ...) ({ \
        static bool __already_done __section(".data.once"); \
        bool ____ret_cond = !!(cond);  \
        if (unlikely(____ret_cond && !__already_done)) \
        {                            \
            __already_done = true;   \
            WARN(1, fmt, ##__VA_ARGS__);           \
        }                            \
        unlikely(____ret_cond);        \
    })

#define WARN_ON_ONCE(cond) ({       \
        static bool __already_done __section(".data.once"); \
        bool ____ret_cond = !!(cond); \
        if (unlikely(____ret_cond && !__already_done)) \
        {                           \
            __already_done = true;  \
            WARN_ON(1);             \
        }                           \
        unlikely(____ret_cond);       \
    })

#endif // KERNEL_BUG_H
