/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/* Copyright(c) 2018-2019  Realtek Corporation
 */

#ifndef __RTW_UTIL_H__
#define __RTW_UTIL_H__

#include <linux/version.h>

struct rtw_dev;

#define rtw_iterate_vifs(rtwdev, iterator, data)                               \
	ieee80211_iterate_active_interfaces(rtwdev->hw,                        \
			IEEE80211_IFACE_ITER_NORMAL, iterator, data)
#define rtw_iterate_vifs_atomic(rtwdev, iterator, data)                        \
	ieee80211_iterate_active_interfaces_atomic(rtwdev->hw,                 \
			IEEE80211_IFACE_ITER_NORMAL, iterator, data)
#define rtw_iterate_stas_atomic(rtwdev, iterator, data)                        \
	ieee80211_iterate_stations_atomic(rtwdev->hw, iterator, data)
#define rtw_iterate_keys(rtwdev, vif, iterator, data)			       \
	ieee80211_iter_keys(rtwdev->hw, vif, iterator, data)

static inline u8 *get_hdr_bssid(struct ieee80211_hdr *hdr)
{
	__le16 fc = hdr->frame_control;
	u8 *bssid;

	if (ieee80211_has_tods(fc))
		bssid = hdr->addr1;
	else if (ieee80211_has_fromds(fc))
		bssid = hdr->addr2;
	else
		bssid = hdr->addr3;

	return bssid;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 16, 0))
extern void __compiletime_error("value doesn't fit into mask")
__field_overflow(void);
extern void __compiletime_error("bad bitfield mask")
__bad_mask(void);
static __always_inline u64 field_multiplier(u64 field)
{
	if ((field | (field - 1)) & ((field | (field - 1)) + 1))
		__bad_mask();
	return field & -field;
}
static __always_inline u64 field_mask(u64 field)
{
	return field / field_multiplier(field);
}
#define field_max(field)	((typeof(field))field_mask(field))
#define ____MAKE_OP(type,base,to,from)					\
static __always_inline __##type type##_encode_bits(base v, base field)	\
{									\
	if (__builtin_constant_p(v) && (v & ~field_mask(field)))	\
		__field_overflow();					\
	return to((v & field_mask(field)) * field_multiplier(field));	\
}									\
static __always_inline __##type type##_replace_bits(__##type old,	\
					base val, base field)		\
{									\
	return (old & ~to(field)) | type##_encode_bits(val, field);	\
}									\
static __always_inline void type##p_replace_bits(__##type *p,		\
					base val, base field)		\
{									\
	*p = (*p & ~to(field)) | type##_encode_bits(val, field);	\
}									\
static __always_inline base type##_get_bits(__##type v, base field)	\
{									\
	return (from(v) & field)/field_multiplier(field);		\
}

#define __MAKE_OP(size)							\
	____MAKE_OP(le##size,u##size,cpu_to_le##size,le##size##_to_cpu)	\
	____MAKE_OP(be##size,u##size,cpu_to_be##size,be##size##_to_cpu)	\
	____MAKE_OP(u##size,u##size,,)
____MAKE_OP(u8,u8,,)
__MAKE_OP(16)
__MAKE_OP(32)
__MAKE_OP(64)
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(4, 16, 0) */

#endif
