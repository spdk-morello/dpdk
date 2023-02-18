/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Cavium, Inc
 */

#ifndef _RTE_PREFETCH_ARM_64_H_
#define _RTE_PREFETCH_ARM_64_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_compat.h>
#include <rte_common.h>
#include "generic/rte_prefetch.h"

static inline void rte_prefetch0(const volatile void *p)
{
#ifdef RTE_ARCH_ARM_PURECAP_HACK
	asm volatile ("PRFM PLDL1KEEP, [%w0]" : : "r" (p));
#else
	asm volatile ("PRFM PLDL1KEEP, [%0]" : : "r" (p));
#endif
}

static inline void rte_prefetch1(const volatile void *p)
{
#ifdef RTE_ARCH_ARM_PURECAP_HACK
	asm volatile ("PRFM PLDL2KEEP, [%w0]" : : "r" (p));
#else
	asm volatile ("PRFM PLDL2KEEP, [%0]" : : "r" (p));
#endif
}

static inline void rte_prefetch2(const volatile void *p)
{
#ifdef RTE_ARCH_ARM_PURECAP_HACK
	asm volatile ("PRFM PLDL3KEEP, [%w0]" : : "r" (p));
#else
	asm volatile ("PRFM PLDL3KEEP, [%0]" : : "r" (p));
#endif
}

static inline void rte_prefetch_non_temporal(const volatile void *p)
{
#ifdef RTE_ARCH_ARM_PURECAP_HACK
	asm volatile ("PRFM PLDL1STRM, [%w0]" : : "r" (p));
#else
	asm volatile ("PRFM PLDL1STRM, [%0]" : : "r" (p));
#endif
}

__rte_experimental
static inline void
rte_cldemote(const volatile void *p)
{
	RTE_SET_USED(p);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTE_PREFETCH_ARM_64_H_ */
