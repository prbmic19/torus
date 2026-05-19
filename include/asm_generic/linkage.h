/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ASM_GENERIC_LINKAGE_H
#define ASM_GENERIC_LINKAGE_H 1

#define SYM_ALIGNMENT 16

#define SYM_BEGIN(_sym)             \
    .balign SYM_ALIGNMENT;          \
    .global _sym;                   \
    _sym:
#define SYM_BEGIN_U(_sym)           \
    .global _sym;                   \
    _sym:

#define SYM_BEGIN_LOCAL(_sym)       \
    .balign SYM_ALIGNMENT;          \
    _sym:
#define SYM_BEGIN_LOCAL_U(_sym) _sym:

#define SYM_END(_sym) .size _sym, . - _sym

#define OBJ_BEGIN(_obj)             \
    .type _obj, @object;            \
    SYM_BEGIN(_obj)
#define OBJ_BEGIN_U(_obj)           \
    .type _obj, @object;            \
    SYM_BEGIN_U(_obj)

#define OBJ_BEGIN_LOCAL(_obj)       \
    .type _obj, @object;            \
    SYM_BEGIN_LOCAL(_obj)
#define OBJ_BEGIN_LOCAL_U(_obj)     \
    .type _obj, @object;            \
    SYM_BEGIN_LOCAL_U(_obj)

#define OBJ_END(_obj) SYM_END(_obj)

#define FUNC_BEGIN(_func)           \
    .type _func, @function;         \
    .balign SYM_ALIGNMENT, 0x90;    \
    .global _func;                  \
    _func:
#define FUNC_BEGIN_U(_func)         \
    .type _func, @function;         \
    SYM_BEGIN_U(_func)              \

#define FUNC_BEGIN_LOCAL(_func)     \
    .type _func, @function;         \
    .balign SYM_ALIGNMENT, 0x90;    \
    _func:
#define FUNC_BEGIN_LOCAL_U(_func)   \
    .type _func, @function;         \
    SYM_BEGIN_LOCAL_U(_func)

#define FUNC_END(_func) SYM_END(_func)

#endif // ASM_GENERIC_LINKAGE_H
